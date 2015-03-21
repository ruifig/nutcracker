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

void DebugSession::removeBreakpoint(Breakpoint* brk)
{
	m_messenger->send(cz::formatString("rb:%x:%s\n", brk->line, brk->file->fullpath.c_str()));
}

void DebugSession::removeWatch(int id)
{
	m_messenger->send(cz::formatString("rw:%d\n", id));
}

void DebugSession::addWatch(int id, const std::string& exp)
{
	m_messenger->send(cz::formatString("aw:%d:%s\n", id, exp.c_str()));
}

void DebugSession::updateBreakpoint(Breakpoint* brk)
{
	if (brk->enabled)
		m_messenger->send(cz::formatString("ab:%x:%s\n", brk->line, brk->file->fullpath.c_str()));
	else
		removeBreakpoint(brk);
}

bool DebugSession::start(const std::string& ip, int port)
{
	if (port)
	{
		m_messenger = std::make_unique<Messenger>();
		if (!m_messenger->connect(ip.c_str(), port, 60*60))
			return false;

		m_messenger->setOnIncoming([this]{ processIncoming(); });

		m_messenger->setOnDisconnected([this]{
			auto this_ = shared_from_this();
			postAppLambdaEvent([this_]
			{
				this_->disconnectListeners.fire();
			});
		});

		gWorkspace->iterateWatches([&](const Watch* watch, const WatchValue* value)
		{
			addWatch(watch->id, watch->name);
		});

		// Setup existing breakpoints
		gWorkspace->_iterateBreakpoints([&](Breakpoint* brk)
		{
			brk->valid = false;
			if (brk->enabled)
				m_messenger->send(cz::formatString("ab:%x:%s\n", brk->line, brk->file->fullpath.c_str()));
		});

		m_messenger->send("rd\n");
	}

	m_paused = false;
	return true;
}

void DebugSession::stepInto()
{
	if (!m_paused)
		return;
	m_messenger->send("si\n");
}

void DebugSession::stepReturn()
{
	if (!m_paused)
		return;
	m_messenger->send("sr\n");
}

void DebugSession::stepOver()
{
	if (!m_paused)
		return;
	m_messenger->send("so\n");
}

void DebugSession::resume()
{
	if (!m_paused)
		return;
	m_messenger->send("go\n");
}

void DebugSession::suspend()
{
	if (m_paused)
		return;
	m_messenger->send("sp\n");
}

void DebugSession::terminate()
{
	m_messenger->send("tr\n");
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

void DebugSession::processMsgResumed(tinyxml2::XMLElement* xml)
{
	// Keep a pointer to this session
	m_paused = false;
	auto this_ = shared_from_this();
	postAppLambdaEvent([this_]
	{
		this_->resumeListeners.fire();
	});
}

void DebugSession::processMsgAddBreakpoint(tinyxml2::XMLElement* xml)
{
	int line;
	CZ_CHECK(xml->QueryAttribute("line", &line) == tinyxml2::XML_SUCCESS);
	auto file = gWorkspace->createFile( getString(xml, "src"));
	CZ_ASSERT(file);
	auto this_ = shared_from_this();
	postAppLambdaEvent([this_, line, file ]
	{
		this_->addBreakpointListeners.fire(line, file);
	});
}

void DebugSession::processMsgBreak(tinyxml2::XMLElement* xml)
{
	m_paused = true;
	auto info = std::make_shared<BreakInfo>();
	CZ_CHECK(xml->QueryAttribute("line", &info->line)==tinyxml2::XML_SUCCESS);
	info->file = gWorkspace->createFile( getString(xml, "src"));
	auto type = xml->Attribute("type");
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
	info->error = getString(xml, "error", "");

	// objs node
	if (auto objs = xml->FirstChildElement("objs"))
	{
		auto entry = objs->FirstChildElement("o");
		while (entry)
		{
			processObjElement(entry, *info);
			entry = entry->NextSiblingElement("o");
		}
	}

	// calls node
	if (auto calls = xml->FirstChildElement("calls"))
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

	auto xml = doc.FirstChildElement();
	if (strcmp(xml->Name(), "resumed") == 0)
		processMsgResumed(xml);
	else if (strcmp(xml->Name(), "addbreakpoint") == 0)
		processMsgAddBreakpoint(xml);
	else if (strcmp(xml->Name(), "break") == 0)
		processMsgBreak(xml);
	else
	{
		czDebug(ID_Log, "Unknown received xml message '%s'", xml->Name());
	}
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
		e.outer = tbl.get();
		tbl->entries.push_back(std::move(e));
		x = x->NextSiblingElement("e");
	}
}

void DebugSession::processCallElement(tinyxml2::XMLElement* ele, BreakInfo& info)
{
	CallstackEntry e;
	e.func = getString(ele, "fnc");
	e.file = gWorkspace->createFile(getString(ele, "src"));
	CZ_CHECK(ele->QueryAttribute("line", &e.line) == tinyxml2::XML_SUCCESS);

	// Local variables
	{
		auto l = ele->FirstChildElement("l");
		while (l)
		{
			TableEntry v;
			v.key = std::make_shared<ValueString>(getString(l, "name"));
			v.val = processValue(l, info, "type", "val");
			e.locals.push_back(std::move(v));
			l = l->NextSiblingElement("l");
		}
	}

	// Watches
	{
		auto w = ele->FirstChildElement("w");
		while (w)
		{
			WatchValue v;
			v.key = std::make_shared<ValueString>(getString(w, "exp"));
			v.id = std::stoi( getString(w, "id") );
			std::string status = getString(w, "status");
			v.state = status == "ok" ? WatchState::Valid : WatchState::Invalid;

			if (v.state==WatchState::Valid)
				v.val = processValue(w, info, "type", "val");

			e.watches[v.id] = std::move(v);
			w = w->NextSiblingElement("w");
		}
	}

	info.callstack.push_back(std::move(e));
}

struct TypeInfo 
{
	const char* dbg;
	VarType type;
	const char* name;
};

static TypeInfo typeInfo[(unsigned int)VarType::MAX] = {
	{ "n", VarType::Null, "null" },
	{ "r", VarType::Root, "root" },
	{ "s", VarType::String, "string" },
	{ "i", VarType::Integer, "integer" },
	{ "f", VarType::Float, "float" },
	{ "u", VarType::UserData, "userdata" },
	{ "fn", VarType::Function, "function" },
	{ "t", VarType::Table, "table" },
	{ "a", VarType::Array, "array" },
	{ "g", VarType::Generator, "generator" },
	{ "h", VarType::Thread, "thread" },
	{ "x", VarType::Instance, "instance" },
	{ "y", VarType::Class, "class" },
	{ "b", VarType::Bool, "bool" },
	{ "w", VarType::WeakRef, "weakref" } };

VarType stringToObjectType(const char* str)
{
	for (const auto& i : typeInfo)
		if (strcmp(str, i.dbg) == 0)
			return i.type;
	CZ_ASSERT_F(0, "Unknown object type '%s'", str);
	return VarType::Null;
};

const char* typeToString(VarType type)
{
	for (const auto& i : typeInfo)
	{
		if (i.type == type)
			return i.name;
	}

	CZ_ASSERT_F(0, "Unknown name for type '%d'", (int)type);
	return "null";
}

std::string TableEntry::getName() const
{
	if (outer == nullptr || outer->type == VarType::Root || outer->type == VarType::Instance ||
		outer->type == VarType::Class)
		return key->txt;


	if (key->type == VarType::String)
		return cz::formatString("[\"%s\"]", key->txt.c_str());
	else
		return cz::formatString("[%s]", key->txt.c_str());
}

std::string TableEntry::getValue() const
{
	if (val->type == VarType::String)
		return cz::formatString("\"%s\"", val->txt.c_str());
	else if (val->type == VarType::Integer || val->type == VarType::Float)
		return val->txt;
	else
		return "";
}

std::string TableEntry::getType() const
{
	if (val->type == VarType::Array || val->type == VarType::Table)
	{
		return cz::formatString("%s[%d]", typeToString(val->type),
			(int)std::static_pointer_cast<ValueBaseTable>(val)->entries.size());
	}
	else
	{
		return typeToString(val->type);
	}
}

//////////////////////////////////////////////////////////////////////////
// Interpreter
//////////////////////////////////////////////////////////////////////////
Interpreter::Interpreter()
{
}

Interpreter::~Interpreter()
{
}

bool Interpreter::hasDebugger() const
{
	return m_hasDebugger;
}

bool Interpreter::launch(const Variables& variables, const UTF8String& file)
{
	auto cwd = wxGetCwd();
	auto cmd = variables.replace(m_launchNormal);
	wxSetWorkingDirectory(Filename(file).getDirectory().c_str());
	auto res = wxExecute(cmd.c_str());
	wxSetWorkingDirectory(cwd);
	if (res == 0)
	{
		czDebug(ID_Log, "Error launching '%s'", cmd.c_str());
		return false;
	}

	return true;
}

std::shared_ptr<DebugSession> Interpreter::launchDebug(const Variables& variables, const UTF8String& file)
{
	auto session = std::make_shared<DebugSession>();

	auto cwd = wxGetCwd();
	auto cmd = variables.replace(m_launchDebug);
	wxSetWorkingDirectory(Filename(file).getDirectory().c_str());
	auto res = wxExecute(cmd.c_str());
	wxSetWorkingDirectory(cwd);
	if (res == 0)
	{
		czDebug(ID_Log, "Error launching '%s'", cmd.c_str());
		return nullptr;
	}

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

const UTF8String& Interpreter::getName() const
{
	return m_name;
}

} // nutcracker



