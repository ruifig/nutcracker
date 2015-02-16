#include "NutcrackerPCH.h"
#include "Interpreter.h"


namespace cz
{
namespace document
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

std::unique_ptr<Session> Interpreter::launch(const Variables& variables, const UTF8String& file)
{
	auto session = std::make_unique<Session>();
	wxExecute(variables.replace(m_launch).c_str());
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
	inter->m_launch = file.getValue<const char*>("General", "launch", "");

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

} // namespace document
} // document cz


