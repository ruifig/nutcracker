#include "NutcrackerPCH.h"
#include "Interpreter.h"
#include "Messenger.h"
#include "Workspace.h"
#include "AppEvents.h" 
#include "NutcrackerApp.h"

namespace nutcracker
{

Variables::Variables(Variables* parent) : m_parent(parent)
{
}

void Variables::set(UTF8String name, GenFunc genFunc)
{
	m_vars[name] = std::move(genFunc);
}

cz::UTF8String Variables::replace(const UTF8String& str) const
{
	UTF8String res = str;
	for (auto& f : m_vars)
		res = stringReplaceAll(res, f.first, f.second());

	if (m_parent)
		res = m_parent->replace(res);

	return res;
}



//////////////////////////////////////////////////////////////////////////
// DebuggerSession
//////////////////////////////////////////////////////////////////////////
DebugSession::DebugSession()
{
}

DebugSession::~DebugSession()
{
}

bool DebugSession::start(const std::string& ip, int port)
{
	if (port)
	{
		m_messenger = std::make_unique<Messenger>();
		if (!m_messenger->connect(ip.c_str(), port, 5))
			return false;

		m_messenger->setOnIncoming([this]{ processIncoming(); });
		m_messenger->send("aw:0:unusedGlobalVariable\n");
		m_messenger->send("aw:1:unusedFunctionVariable\n");
		m_messenger->send("aw:2:someArray\n");
		m_messenger->send("aw:3:someClass\n");
		m_messenger->send("rd\n");
	}

	return true;
}

const char* getString(tinyxml2::XMLElement* ele, const char* name, const char* defValue=0)
{
	auto s = ele->Attribute(name);
	if (s)
		return s;
	else  if (defValue)
		return defValue;
	else
	{
		CZ_UNEXPECTED();
		return "";
	}
}

void DebugSession::processIncoming()
{
	std::string msg;
	if (!m_messenger->read(msg))
		return;

	tinyxml2::XMLDocument doc;
	if (doc.Parse(msg.c_str()) != tinyxml2::XML_SUCCESS)
	{
		czDebug(ID_Log, "Error parsing xml message.");
		return;
	}

	/*
	{
		FILE* fp = fopen("doc.txt", "w+");
		tinyxml2::XMLPrinter printer(fp);
		doc.Print(&printer);
		fclose(fp);
	}
	*/

	auto brk = doc.FirstChildElement("break");
	if (!brk)
	{
		czDebug(ID_Log, "Received xml message does not have the 'break' node");
		return;
	}

	auto info = std::make_shared<BreakInfo>();
	CZ_CHECK(brk->QueryAttribute("line", &info->line)==tinyxml2::XML_SUCCESS);
	info->file = gWorkspace->files.createFile( getString(brk, "src"));
	auto type = brk->Attribute("type");
	CZ_CHECK(type);
	if (strcmp(type, "step") == 0)
		info->type = BreakType::Step;
	else if (strcmp(type, "breakpoint") == 0)
		info->type = BreakType::Breakpoint;
	else if (strcmp(type, "error") == 0)
		info->type = BreakType::Error;
	else
	{
		CZ_UNEXPECTED();
	}
	info->error = getString(brk, "error", "");

	// objs node
	if (auto objs = brk->FirstChildElement("objs"))
	{
		auto entry = objs->FirstChildElement("o");
		while (entry)
		{
			processObjElement(entry, *info);
			entry = entry->NextSiblingElement("o");
		}
	}

	// calls node
	if (auto calls = brk->FirstChildElement("calls"))
	{
		auto entry = calls->FirstChildElement("call");
		while (entry)
		{
			processCallElement(entry, *info);
			entry = entry->NextSiblingElement("call");
		}
	}
	
	// Keep a pointer to this session
	auto this_ = shared_from_this();
	postAppLambdaEvent([info, this_]
	{
		this_->breakListeners.fire(info);
	});

}

std::shared_ptr<ValueBase> DebugSession::processValue(tinyxml2::XMLElement* ele, BreakInfo& info, const char* attrType, const char* attrVal)
{
	auto type = getString(ele, attrType);
	std::shared_ptr<ValueBase> base;
	const char* val = "";
	if (attrVal)
		val = getString(ele, attrVal);
	if (strcmp(type, "n") == 0)
		base = std::make_shared<ValueNull>();
	else if (strcmp(type, "r") == 0)
		base = std::make_shared<ValueRoot>();
	else if (strcmp(type, "s") == 0)
		base = std::make_shared<ValueString>(val);
	else if (strcmp(type, "i") == 0)
		base = std::make_shared<ValueInteger>(std::stoi(val));
	else if (strcmp(type, "f") == 0)
		base = std::make_shared<ValueFloat>(std::stof(val));
	else if (strcmp(type, "u") == 0)
		base = std::make_shared<ValueUserData>();
	else if (strcmp(type, "fn") == 0)
		base = std::make_shared<ValueFunction>();
	else if (strcmp(type, "t") == 0)
		base = std::make_shared<ValueTable>();
	else if (strcmp(type, "a") == 0)
		base = std::make_shared<ValueArray>();
	else if (strcmp(type, "g") == 0)
		base = std::make_shared<ValueGenerator>();
	else if (strcmp(type, "h") == 0)
		base = std::make_shared<ValueThread>();
	else if (strcmp(type, "x") == 0)
		base = std::make_shared<ValueInstance>();
	else if (strcmp(type, "y") == 0)
		base = std::make_shared<ValueClass>();
	else if (strcmp(type, "b") == 0)
		base = std::make_shared<ValueBool>(strcmp(val,"true")==0 ? true : false);
	else if (strcmp(type, "w") == 0)
		base = std::make_shared<ValueWeakRef>();
	else
	{
		CZ_ASSERT_F(0, "Unknown type '%s'", type);
		return nullptr;
	}

	if (base->isTable() && attrVal)
	{
		int key = std::stoi(val);
		auto& obj = info.objs[key];
		if (obj)
			base = obj; // There is an entry with this key, so return that object
		else
			obj = base; // We are referecing an entry that is not yet available, so create it
	}

	return base;
}

void DebugSession::processObjElement(tinyxml2::XMLElement* ele, BreakInfo& info)
{
	czDebug(ID_Log, "obj: type=\"%s\" ref=\"%s\"", getString(ele, "type"), getString(ele, "ref"));
	
	// Because the "obj" entries dependencies are not necessarly ordered (an "e" entry might reference an obj that is further
	// down the document), it means those forward references were created when reading the "e" entry that references it.
	// This means we might already have it in the map
	auto ref = ele->IntAttribute("ref");
	auto& obj = info.objs[ref];
	if (!obj)
	{
		obj = processValue(ele, info, "type");
		czDebug(ID_Log, "objs[%d] = %s", ref, obj->txt.c_str());
	}
	CZ_ASSERT_F(obj->isTable(), "'obj' element needs to some kind of table.");
	auto tbl = std::static_pointer_cast<ValueBaseTable>(obj);


	auto x = ele->FirstChildElement("e");
	while (x)
	{
		TableEntry e;
		e.key = processValue(x, info, "kt", "kv");
		e.val = processValue(x, info, "vt", "v");
		tbl->entries.push_back(std::move(e));
		x = x->NextSiblingElement("e");
	}
}

void DebugSession::processCallElement(tinyxml2::XMLElement* ele, BreakInfo& info)
{
	CallstackEntry e;
	e.func = getString(ele, "fnc");
	e.file = gWorkspace->files.createFile(getString(ele, "src"));
	CZ_CHECK(ele->QueryAttribute("line", &e.line) == tinyxml2::XML_SUCCESS);

	// Local variables
	auto l = ele->FirstChildElement("l");
	while (l)
	{
		Variable v;
		v.name = getString(l, "name");
		v.val = processValue(l, info, "type", "val");
		e.vars.push_back(std::move(v));
		l = l->NextSiblingElement("l");
	}

	info.callstack.push_back(std::move(e));
}

VarType stringToObjectType(const char* str)
{
	static std::pair<const char*, VarType> data[(unsigned int)VarType::MAX] = {
		{ "n", VarType::Null },
		{ "s", VarType::String },
		{ "i", VarType::Integer },
		{ "f", VarType::Float },
		{ "u", VarType::UserData },
		{ "fn",VarType::Function},
		{ "t", VarType::Table },
		{ "a", VarType::Array },
		{ "g", VarType::Generator },
		{ "h", VarType::Thread },
		{ "x", VarType::Instance },
		{ "y", VarType::Class },
		{ "b", VarType::Bool },
		{ "w", VarType::WeakRef } };

	for (const auto& i : data)
		if (strcmp(str, i.first) == 0)
			return i.second;
	CZ_ASSERT_F(0, "Unknown object type '%s'", str);
	return VarType::Null;
};

//////////////////////////////////////////////////////////////////////////
// Interpreter
//////////////////////////////////////////////////////////////////////////
Interpreter::Interpreter()
{
}

Interpreter::~Interpreter()
{
}

bool Interpreter::hasDebugger()
{
	return m_hasDebugger;
}

std::shared_ptr<DebugSession> Interpreter::launch(const Variables& variables, const UTF8String& file, bool debug)
{
	auto session = std::make_shared<DebugSession>();

	auto cwd = wxGetCwd();
	auto cmd = variables.replace(debug ? m_launchDebug : m_launchNormal);
	wxSetWorkingDirectory(Filename(file).getDirectory().c_str());
	wxExecute(cmd.c_str());
	wxSetWorkingDirectory(cwd);

	if (!session->start(m_ip, m_port))
		return nullptr;

	return session;
}

std::unique_ptr<Interpreter> Interpreter::init(const UTF8String& cfgFile)
{
	IniFile file;
	if (!file.open(cfgFile.c_str()))
	{
		czWarning(ID_Log, "Error loading interpreter config file '%s'", cfgFile.c_str());
		return nullptr;
	}

	auto inter = std::make_unique<Interpreter>();
	auto cfgGeneral = file.getSection("General");

	inter->m_hasDebugger = file.getValue<bool>("General", "debugger", false);
	inter->m_name = file.getValue<const char*>("General", "name", "Unknown");
	inter->m_launchNormal = file.getValue<const char*>("General", "launch_normal", "");
	inter->m_launchDebug = file.getValue<const char*>("General", "launch_debug", "");
	inter->m_ip = file.getValue<const char*>("Debugger", "ip", "127.0.0.1");
	inter->m_port = file.getValue<int>("Debugger", "port", 0);

	return std::move(inter);
}

std::vector<std::unique_ptr<Interpreter>> Interpreter::initAll(const UTF8String& folder)
{
	decltype(initAll(folder)) res;
	auto files = Filesystem::getSingleton().getFilesInDirectory(folder, "*.ini", false);

	for (auto& f : files)
	{
		auto interpreter = init(folder + f.name);
		if (interpreter)
			res.push_back(std::move(interpreter));
	}

	return res;
}

const UTF8String& Interpreter::getName()
{
	return m_name;
}

} // nutcracker



