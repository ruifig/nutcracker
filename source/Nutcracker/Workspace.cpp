/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com
	
	purpose:
	
*********************************************************************/

#include "NutcrackerPCH.h"
#include "Workspace.h"
#include "Interpreter.h"
#include "UIDefs.h"

namespace nutcracker
{

static bool getConfigEntry(IniFile& file, const char* section, const char* entry, UTF8String& dst)
{
	UTF8String val = file.getValue(section, entry, "");
	if (val == "")
	{
		showError("Error loading config file", "Config entry '[%s]:%s' is missing.", section, entry);
		return false;
	}
	dst = val;
	return true;
}


typedef std::map<std::string, std::vector<std::string>> ConfigSaveInfo;

struct ConfigEntry
{
	enum Type
	{
		String,
		Boolean
	};

	ConfigEntry(const char* section, const char* name, UTF8String* var)
		: section(section), name(name), var(var), type(String) { }
	ConfigEntry(const char* section, const char* name, bool* var)
		: section(section), name(name), var(var), type(Boolean) { }

	bool load(cz::IniFile& file)
	{
		switch (type)
		{
			case String:
				if (!getConfigEntry(file, section, name, *(UTF8String*)var))
					return false;
				break;
			case Boolean:
				*((bool*)var) = file.getValue(section, name, false);
				break;
			default:
				CZ_UNEXPECTED();
		}
		return true;
	}

	void save(ConfigSaveInfo& out)
	{
		switch (type)
		{
			case String:
				out[section].push_back(cz::formatString("%s=\"%s\"\n", name, ((UTF8String*)var)->c_str()));
				break;
			case Boolean:
				out[section].push_back(cz::formatString("%s=%s\n", name, *(bool*)var ? "true" : "false"));
				break;
			default:
				CZ_UNEXPECTED();
		}
	}

	const char* section;
	const char* name;
	void* var;
	Type type;
};


static std::vector<ConfigEntry> getConfig(Options& obj)
{
	return
	{
		{"General", "defaultInterpreter", &obj.general_defaultInterpreter},
		{ "View", "indentation", &obj.view_indentation },
		{ "View", "whitespaces", &obj.view_whitespaces },
		{ "View", "eol", &obj.view_eol }
	};
}

Workspace::Workspace() : m_files(this), m_breakpoints(this)
{
}

void Workspace::registerListener(void* listener, std::function<void(const DataEvent&)> func)
{
	auto f = [this, listener, func]() { m_listeners.emplace_back(listener, std::move(func)); };
	if (m_inEventHandler)
		m_pendingFuncs.push_back(std::move(f));
	else
		f();
}

void Workspace::removeListener(void* listener)
{
	CZ_ASSERT(m_inEventHandler == 0);
	auto f = [this, listener]
	{
		cz::erase_if(m_listeners, [&](decltype(m_listeners[0])& i)
		{
			return i.first == listener;
		});
	};

	if (m_inEventHandler)
		m_pendingFuncs.push_back(std::move(f));
	else
		f();
}

void Workspace::fireEvent(const DataEvent& evt)
{
	m_inEventHandler++;
	for (auto& i : m_listeners)
		i.second(evt);
	m_inEventHandler--;

	// Only add or remove listeners when we don't have any more nested fireEvent calls
	if (m_inEventHandler == 0)
	{
		if (m_pendingFuncs.size())
		{
			for (auto& f : m_pendingFuncs)
				f();
			m_pendingFuncs.clear();
		}
	}
}

void Workspace::fireEvent(const DataEventID id)
{
	fireEvent(DataEvent(id));
}

std::shared_ptr<const File> Workspace::getFile(FileId fileId)
{
	return m_files.getFile(fileId);
}

std::shared_ptr<const File> Workspace::createFile(UTF8String path)
{
	return m_files.createFile(path);
}

void Workspace::addFolder(const UTF8String& path)
{
	m_files.addFolder(path);
	fireEvent(DataEventID::WorkspaceChanges);
}

void Workspace::resyncFolders()
{
	m_files.resyncFolders();
}

std::shared_ptr<const Folder> Workspace::getRoot()
{
	return m_files.getRoot();
}

void Workspace::setFileDirty(FileId fileId, bool dirty)
{
	auto file = m_files.getFile(fileId);
	CZ_ASSERT(file);
	bool fire = (dirty && file->dirty == false);
	file->dirty = dirty;
	if (fire)
		fireEvent(FileDirty(file));
}

void Workspace::setFileSaveFunc(FileId fileId, std::function<bool(const std::shared_ptr<const File>& file)> func)
{
	auto file = m_files.getFile(fileId);
	CZ_ASSERT(file);
	file->m_saveFunc = std::move(func);
}

void Workspace::removeFileSaveFunc(FileId fileId)
{
	auto file = m_files.getFile(fileId);
	CZ_ASSERT(file);
	file->m_saveFunc = nullptr;
}

bool Workspace::saveFile(FileId fileId)
{
	auto file = m_files.getFile(fileId);
	CZ_ASSERT(file);

	CZ_ASSERT_F(file->m_saveFunc, "No save function specified for file '%s'", file->fullpath.c_str());

	if (!file->m_saveFunc(file))
		return false;
	file->dirty = false;
	file->filetime = FileTime::get(file->fullpath, FileTime::kModified);
	m_breakpoints.iterate(file, [&](Breakpoint* brk)
	{
		brk->savedline = brk->line;
	});
	fireEvent(FileSaved(file));

	return true;
}

void Workspace::iterateFiles(std::function<void(const std::shared_ptr<const File>&)>&& func)
{
	m_files.iterateFiles(func);
}

const Breakpoint* Workspace::addBreakpoint(FileId fileId, int line, int markerHandler)
{
	auto file = m_files.getFile(fileId);
	CZ_ASSERT(file);
	auto brk = m_breakpoints.add(file, line, markerHandler);
	fireEvent(BreakpointAdd(brk));
	return brk;
}

int Workspace::getBreakpointCount()
{
	return m_breakpoints.getCount();
}

void Workspace::iterateBreakpoints(FileId fileId, std::function<void(const Breakpoint* brk)> func)
{
	auto file = m_files.getFile(fileId);
	CZ_ASSERT(file);
	m_breakpoints.iterate(file, std::move(func));
}

void Workspace::iterateBreakpoints(std::function<void(const Breakpoint* brk)> func)
{
	m_breakpoints.iterate(std::move(func));
}

const Breakpoint* Workspace::getBreakpoint(int index)
{
	return m_breakpoints.getAt(index);
}

const Breakpoint* Workspace::toggleBreakpoint(Breakpoint* brk)
{
	brk->enabled = !brk->enabled;
	fireEvent(BreakpointToggle(brk));
	return brk;
}

const Breakpoint* Workspace::toggleBreakpoint(int index)
{
	auto b = m_breakpoints.getAt(index);
	return toggleBreakpoint(b);
}

const Breakpoint* Workspace::toggleBreakpoint(FileId fileId, int line)
{
	auto file = m_files.getFile(fileId);
	CZ_ASSERT(file);
	auto b = m_breakpoints.get(file, line);
	CZ_ASSERT(b);
	return toggleBreakpoint(b);
}

void Workspace::dropBreakpointChanges(FileId fileId)
{
	auto file = m_files.getFile(fileId);
	CZ_ASSERT(file);
	m_breakpoints.iterate(file, [&](Breakpoint* brk)
	{
		brk->markerHandle = -1;
		brk->line = brk->savedline;
		fireEvent(BreakpointDropChanges(brk));
	});
}

void Workspace::setBreakpointPos(const Breakpoint* brk, int line, int markerHandler)
{
	Breakpoint* b = const_cast<Breakpoint*>(brk);
	bool fire = b->line != line;
	b->line = line;
	b->markerHandle = markerHandler;
	if (fire)
		fireEvent(BreakpointUpdated(brk));
}

void Workspace::removeBreakpoint(FileId fileId, int line)
{
	auto file = m_files.getFile(fileId);
	CZ_ASSERT(file);
	auto brk = m_breakpoints.remove(file, line);
	if (brk)
		fireEvent(BreakpointRemoved(brk.get()));
}

static Variables getVariables(File* file)
{
	Variables vars;
	vars.set("%FILE%", [file]()
	{
		return UTF8String("\"") + file->fullpath + "\"";
	});
	return vars;
}

void Workspace::doDebugStop()
{
	m_debugSession = nullptr;
	m_breakInfo = nullptr;
	fireEvent(DataEventID::DebugStop);
}

void Workspace::doDebugResume()
{
	m_breakInfo = nullptr;
	fireEvent(DataEventID::DebugResume);
}

bool Workspace::debuggerStart(FileId fileId)
{
	auto file = m_files.getFile(fileId);
	CZ_ASSERT(file);

	auto vars = getVariables(file.get());

	m_debugSession = _getCurrentInterpreter()->launchDebug(vars, file->fullpath);
	if (!m_debugSession)
		return false;

	m_debugSession->disconnectListeners.add(this, [this]()
	{
		doDebugStop();
	});

	m_debugSession->breakListeners.add(this, [this](const std::shared_ptr<BreakInfo>& info)
	{
		m_breakInfo = info;
		fireEvent(DataEventID::DebugBreak);
	});

	m_debugSession->resumeListeners.add(this, [this]()
	{
		doDebugResume();
	});

	fireEvent(DataEventID::DebugStart);
	return true;
}


bool Workspace::run(FileId fileId)
{
	auto file = m_files.getFile(fileId);
	CZ_ASSERT(file);
	auto vars = getVariables(file.get());
	auto res = _getCurrentInterpreter()->launch(vars, file->fullpath);
	return res;
}

bool Workspace::debuggerActive()
{
	return m_debugSession ? true : false;
}

const BreakInfo* Workspace::debuggerGetBreakInfo()
{
	return m_breakInfo ? m_breakInfo.get() : nullptr;
}

void Workspace::debuggerSetCallstackFrame(int index)
{
	if (!m_breakInfo || (static_cast<size_t>(index)>=m_breakInfo->callstack.size()))
		return;

	m_breakInfo->currentCallstackFrame = index;
	fireEvent(DataEventID::DebugChangedCallstackFrame);
}

void Workspace::debuggerResume()
{
	if (!m_debugSession)
		return;
	m_debugSession->resume();
}

void Workspace::debuggerStepOver()
{
	if (!m_debugSession)
		return;
	m_debugSession->stepOver();
}

void Workspace::debuggerStepInto()
{
	if (!m_debugSession)
		return;
	m_debugSession->stepInto();
}

void Workspace::debuggerStepReturn()
{
	if (!m_debugSession)
		return;
	m_debugSession->stepReturn();
}

void Workspace::debuggerTerminate()
{
	if (!m_debugSession)
		return;
	m_debugSession->terminate();
	doDebugStop();
}

void Workspace::debuggerSuspend()
{
	if (!m_debugSession)
		return;
	m_debugSession->suspend();
}

void Workspace::loadConfig()
{
	IniFile file;
	auto cfgFile = "config.ini";
	if (!file.open("config.ini"))
		showError("Error loading config file '%s'", cfgFile);

	Options options;
	auto cfg = getConfig(options);
	for (auto& i : cfg)
	{
		if (!i.load(file))
			return;
	}
	m_options = options;
}

void Workspace::saveConfig()
{
	auto cfg = getConfig(m_options);
	cz::IniFile ini;

	ConfigSaveInfo saveInfo;

	for (auto& i : cfg)
		i.save(saveInfo);

	cz::File file;
	if (!file.try_open("config.ini", cz::File::FILEMODE_WRITE))
		return;
	auto write = [&file](const char* str)
	{
		file.write(str, strlen(str));
	};

	for (auto& s : saveInfo)
	{
		write(cz::formatString("[%s]\n", s.first.c_str()));
		for (auto& e : s.second)
			write(e.c_str());
		write("\n");
	}

}

void Workspace::setViewIdentation(bool enabled)
{
	m_options.view_indentation = enabled;
	fireEvent(DataEventID::ViewIndentation);
}

void Workspace::setViewWhitespaces(bool enabled)
{
	m_options.view_whitespaces = enabled;
	fireEvent(DataEventID::ViewWhitespaces);
}

void Workspace::setViewEOL(bool enabled)
{
	m_options.view_eol = enabled;
	fireEvent(DataEventID::ViewEOL);
}

const Options* Workspace::getViewOptions()
{
	return &m_options;
}

void Workspace::loadInterpreters()
{
	m_interpreters.all = Interpreter::initAll(Filesystem::getSingleton().getCWD() + "interpreters\\");

	m_interpreters.current = 0;
	for (int i=0; i<(int)m_interpreters.all.size(); i++)
	{
		if (m_interpreters.all[i]->getName() == m_options.general_defaultInterpreter)
		{
			m_interpreters.current = i;
			break;
		}
	}
}

int Workspace::getNumInterpreters()
{
	return m_interpreters.all.size();
}

const Interpreter* Workspace::getInterpreter(int index)
{
	return m_interpreters.all[index].get();
}

const Interpreter* Workspace::getCurrentInterpreter()
{
	return m_interpreters.all[m_interpreters.current].get();
}

Interpreter* Workspace::_getCurrentInterpreter()
{
	return m_interpreters.all[m_interpreters.current].get();
}


void Workspace::setCurrentInterpreter(int index)
{
	CZ_ASSERT(static_cast<size_t>(index) < m_interpreters.all.size());
	m_interpreters.current = index;
	m_options.general_defaultInterpreter = _getCurrentInterpreter()->getName();
	fireEvent(DataEventID::InterpreterChanged);
}


} // namespace nutcracker
