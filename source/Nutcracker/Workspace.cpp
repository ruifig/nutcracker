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

FindResult::FindResult(wxString t_, const std::shared_ptr<const File>& f_, int l_, int c_)
	: text(std::move(t_)), file(std::move(f_))
{
	pos.line = l_;
	pos.col = c_;
	savedPos = pos;
}

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


static UTF8String getConfigFilePath()
{
	return cz::Filesystem::getSingleton().getExecutableDirectory() + "config.ini";
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

	if (evt.makesWorkspaceDirty && !m_isDirty)
	{
		m_isDirty = true;
		fireEvent(DataEventID::WorkspaceDirty, false);
	}
}

void Workspace::fireEvent(const DataEventID id, bool makesWSDirty)
{
	fireEvent(DataEvent(id, makesWSDirty));
}

std::shared_ptr<const File> Workspace::getFile(FileId fileId)
{
	return m_files.getFile(fileId);
}

std::shared_ptr<const BaseItem> Workspace::getItem(FileId itemId)
{
	return m_files.getItem(itemId);
}

std::shared_ptr<const Folder> Workspace::getFolder(FileId folderId)
{
	return m_files.getFolder(folderId);
}

std::shared_ptr<const File> Workspace::createFile(UTF8String path)
{
	return m_files.createFile(path);
}

void Workspace::addFolder(const UTF8String& path)
{
	m_files.addFolder(path);
	fireEvent(DataEventID::WorkspaceChanges, true);
}

void Workspace::removeFolder(const UTF8String& path)
{
	m_files.removeFolder(path);
	fireEvent(DataEventID::WorkspaceChanges, true);
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
	{
		fireEvent(FileDirty(file));
		fireEvent(DataEventID::WorkspaceDirty, false);
	}
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

	for (auto& f : m_findResults)
	{
		if (f.file!=file)
			continue;
		f.savedPos = f.pos;
	}

	fireEvent(FileSaved(file));
	fireEvent(DataEventID::WorkspaceDirty, false);

	return true;
}

void Workspace::iterateFiles(std::function<bool(const std::shared_ptr<const File>&)>&& func)
{
	m_files.iterateFiles(func);
}

const Breakpoint* Workspace::addBreakpoint(FileId fileId, int line, int markerHandler, bool enabled)
{
	auto file = m_files.getFile(fileId);
	CZ_ASSERT(file);
	auto brk = m_breakpoints.add(file, line, markerHandler, enabled);
	if (debuggerActive())
		m_debugSession->updateBreakpoint(brk);
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

void Workspace::_iterateBreakpoints(std::function<void(Breakpoint* brk)> func)
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
	if (debuggerActive())
		m_debugSession->updateBreakpoint(brk);
	fireEvent(BreakpointToggle(brk));
	return brk;
}

const Breakpoint* Workspace::toggleBreakpoint(int index)
{
	auto b = m_breakpoints.getAt(index);
	if (b)
		return toggleBreakpoint(b);
	else
		return nullptr;
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

void Workspace::doRemoveBreakpoint(const std::unique_ptr<Breakpoint>& brk)
{
	if (brk)
	{
		if (debuggerActive())
			m_debugSession->removeBreakpoint(brk.get());
		fireEvent(BreakpointRemoved(brk.get()));
	}
}

void Workspace::removeBreakpoint(int index)
{
	auto brk = m_breakpoints.removeAt(index);
	doRemoveBreakpoint(brk);
}

void Workspace::removeBreakpoint(FileId fileId, int line)
{
	auto file = m_files.getFile(fileId);
	CZ_ASSERT(file);
	auto brk = m_breakpoints.remove(file, line);
	doRemoveBreakpoint(brk);
}

int Workspace::addWatch(std::string watch, int id)
{
	Watch w;
	if (id)
		w.id = id;
	else
		w.id = m_watchIDCounter++;
	w.name = std::move(watch);

	if (debuggerActive())
	{
		// If we are editing an existing watch, then remove the previous one, as it seems the default SQDBG protocol
		// implementation in squirrel doesn't update existing watches
		if (id)
			m_debugSession->removeWatch(id);
		m_debugSession->addWatch(w.id, w.name);
	}

	// Sync the watch list with the breakpoint info
	if (m_breakInfo)
	{
		for (auto& callstack : m_breakInfo->callstack)
		{
			auto& e = callstack.watches[w.id];
			if (e.id == 0 || e.getName()!=w.name)
			{
				e.key = std::make_shared<ValueString>(w.name.c_str());
				e.id = w.id;
				e.state = WatchState::Unknown;
				e.val.reset();
			}
		}
	}

	auto it = cz::find_if(m_watches, [id](Watch& w)
	{
		return w.id == id;
	});

	int ret = w.id;
	if (it != m_watches.end())
		*it = std::move(w);
	else
		m_watches.push_back(std::move(w));

	fireEvent(DataEventID::WatchesChanged, true);
	return ret;
}


int Workspace::getWatchCount() const
{
	return static_cast<int>(m_watches.size());
}

void Workspace::iterateWatches(std::function<void(const struct Watch*, const WatchValue*)> func)
{
	if (m_breakInfo)
	{
		auto& watches = m_breakInfo->callstack[m_breakInfo->currentCallstackFrame].watches;
		for (const auto& w : m_watches)
		{
			assert(w.id != 0);
			auto& e = watches[w.id];
			assert(e.id == w.id);
			func(&w, &e);
		}
	}
	else
	{
		WatchValue e;
		e.key = std::make_shared<ValueString>("");
		for (const auto& w : m_watches)
		{
			e.key->txt = w.name;
			func(&w, &e);
		}
	}
}

const Watch* Workspace::getWatchByID(int id)
{
	for (auto it = m_watches.begin(); it != m_watches.end(); it++)
	{
		if (it->id == id)
			return &(*it);
	}

	return nullptr;
}

void Workspace::removeWatchByID(int id)
{
	for (auto it = m_watches.begin(); it != m_watches.end(); it++)
	{
		if (it->id == id)
		{
			m_watches.erase(it);
			if (debuggerActive())
				m_debugSession->removeWatch(id);

			// Sync the watch list with the breakpoint info
			if (m_breakInfo)
			{
				for (auto& callstack : m_breakInfo->callstack)
					callstack.watches.erase(id);
			}

			fireEvent(DataEventID::WatchesChanged, true);
			break;
		}
	}
}

static Variables getVariables(File* file)
{
	Variables vars;
	vars.set("%FILE%", [file]()
	{
		return UTF8String("\"") + file->fullpath + "\"";
	});
	vars.set("%NUTCRACKERDIR%", []()
	{
		return cz::Filesystem::getSingleton().getExecutableDirectory();
	});
	return vars;
}

void Workspace::doDebugStop()
{
	m_debugSession = nullptr;
	m_breakInfo = nullptr;
	fireEvent(DataEventID::DebugStop, false);
}

void Workspace::doDebugResume()
{
	m_breakInfo = nullptr;
	fireEvent(DataEventID::DebugResume, false);
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
		fireEvent(DataEventID::DebugBreak, false);
	});

	m_debugSession->resumeListeners.add(this, [this]()
	{
		doDebugResume();
	});

	m_debugSession->addBreakpointListeners.add(this, [this](int line, std::shared_ptr<const File> file)
	{
		auto brk = m_breakpoints.get(file, line);
		if (brk)
		{
			brk->valid = true;
			fireEvent(BreakpointValidated(brk));
		}
	});

	fireEvent(DataEventID::DebugStart, false);
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
	fireEvent(DataEventID::DebugChangedCallstackFrame, false);
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
	auto cfgFile = getConfigFilePath();
	if (!file.open(cfgFile.c_str()))
		showError("Error loading config file '%s'", cfgFile.c_str());

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
	auto cfgFile = getConfigFilePath();
	if (!file.try_open(cfgFile.c_str(), cz::File::FILEMODE_WRITE))
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
	fireEvent(DataEventID::ViewIndentation, false);
}

void Workspace::setViewWhitespaces(bool enabled)
{
	m_options.view_whitespaces = enabled;
	fireEvent(DataEventID::ViewWhitespaces, false);
}

void Workspace::setViewEOL(bool enabled)
{
	m_options.view_eol = enabled;
	fireEvent(DataEventID::ViewEOL, false);
}

const Options* Workspace::getViewOptions()
{
	return &m_options;
}

void Workspace::loadInterpreters()
{
	m_interpreters.all = Interpreter::initAll(Filesystem::getSingleton().getExecutableDirectory() + "..\\interpreters\\");

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
	fireEvent(DataEventID::InterpreterChanged, false);
}

cz::UTF8String Workspace::getName() const
{
	if (m_filename.size() == 0)
		return "untitled";
	else
		return m_filename.getBaseFilename();
}

cz::UTF8String Workspace::getFullPath() const
{
	return m_filename.getFullPath();
}

bool Workspace::isDirty()
{
	if (m_isDirty || m_files.hasDirtyFiles())
		return true;
	else
		return false;
}

void Workspace::clearFindResults()
{
	m_findResults.clear();
	fireEvent(FindResultsClear());
}

int Workspace::getFindResultsCount()
{
	return static_cast<int>(m_findResults.size());
}

void Workspace::addFindResult(wxString txt, std::shared_ptr<const File> file, int line, int col)
{
	m_findResults.push_back(FindResult(std::move(txt), std::move(file), line, col));
	fireEvent(FindResultsAdd(&m_findResults.back()));
}

/*
void Workspace::updateFindResultPos(const FindResult* result, int line, int col, int markerHandler)
{
	auto res = const_cast<FindResult*>(result);
	bool fire = res->pos.line != line || res->pos.col != col;
	res->pos.line = line;
	res->pos.col = col;
	res->markerHandle = markerHandler;
	if (fire)
		fireEvent(FindResultsUpdated(result));
}
*/

void Workspace::iterateFindResult(std::function<void(const FindResult*)> func)
{
	for (auto& f : m_findResults)
		func(&f);
}

void Workspace::doSave(tinyxml2::XMLDocument& doc)
{

	auto createElement = [&](tinyxml2::XMLElement* parent, const char* name, const char* text)
	{
		auto ele = doc.NewElement(name);
		if (parent)
			parent->InsertEndChild(ele);
		else
			doc.InsertEndChild(ele);
		if (text && text[0]!=0)
			ele->SetText(text);
		return ele;
	};

	//
	// Files
	{
		auto foldersEle = createElement(nullptr, "folders", "");
		for (auto& item : m_files.getRoot()->items)
		{
			if (item->type != ItemType::Folder)
				return;

			auto folderEle = createElement(foldersEle, "folder", "");
			createElement(folderEle, "path", item->fullpath.c_str());
		}
	}

	//
	// Breakpoints
	{
		auto breakpointsEle = createElement(nullptr, "breakpoints", "");
		m_breakpoints.iterate([&](Breakpoint* brk)
		{
			auto breakpointEle = createElement(breakpointsEle, "breakpoint", "");
			createElement(breakpointEle, "file", brk->file->fullpath.c_str());
			createElement(breakpointEle, "line", std::to_string(brk->line).c_str());
			createElement(breakpointEle, "enabled", brk->enabled ? "1" : "0");
		});
	}

	//
	// Watches
	{
		auto watchesEle = createElement(nullptr, "watches", "");
		for (const auto& w : m_watches)
		{
			createElement(watchesEle, "watch", w.name.c_str());
		}
	}

}

void iterateElements(tinyxml2::XMLElement* ele, const char* name, std::function<void(tinyxml2::XMLElement*)> func)
{
	auto entry = ele->FirstChildElement(name);
	while (entry)
	{
		func(entry);
		entry = entry->NextSiblingElement(name);
	}
}

void Workspace::doLoad(tinyxml2::XMLDocument& doc)
{
	close();

	auto doCheck = [](bool val)
	{
		if (!val)
			throw std::runtime_error("Error loading workspace");
	};

	auto getSimpleElementAsText = [&](tinyxml2::XMLElement* ele, const char* name)
	{
		auto entry = ele->FirstChildElement(name);
		doCheck(entry != nullptr);
		return entry->GetText();
	};
	auto getSimpleElementAsInt = [&](tinyxml2::XMLElement* ele, const char* name)
	{
		auto entry = ele->FirstChildElement(name);
		doCheck(entry != nullptr);
		return std::atoi(entry->GetText());
	};
	auto getSimpleElementAsBool = [&](tinyxml2::XMLElement* ele, const char* name)
	{
		auto entry = ele->FirstChildElement(name);
		doCheck(entry != nullptr);
		return std::atoi(entry->GetText()) == 1 ? true : false;
	};

	//
	// Load files
	{
		auto foldersEle = doc.FirstChildElement("folders");
		doCheck(foldersEle != nullptr);
		iterateElements(foldersEle, "folder", [&](tinyxml2::XMLElement* ele)
		{
			addFolder(getSimpleElementAsText(ele, "path"));
		});
	}

	//
	// Load breakpoints
	{
		auto breakpointsEle = doc.FirstChildElement("breakpoints");
		doCheck(breakpointsEle != nullptr);
		iterateElements(breakpointsEle, "breakpoint", [&](tinyxml2::XMLElement* ele)
		{
			auto path = getSimpleElementAsText(ele, "file");
			auto line = getSimpleElementAsInt(ele, "line");
			bool enabled = getSimpleElementAsBool(ele, "enabled");
			auto file = createFile(path);
			if (!file)
			{
				czDebug(ID_Log, "Ignoring invalid breakpoint: '%s':%d", path, line);
				return;
			}
			addBreakpoint(file->id, line, -1, enabled);
		});
	}


	//
	// Watches
	{
		auto watchesEle = doc.FirstChildElement("watches");
		doCheck(watchesEle != nullptr);
		iterateElements(watchesEle, "watch", [&](tinyxml2::XMLElement* ele)
		{
			addWatch(ele->GetText());
		});
	}

}

void Workspace::save(const UTF8String& filename)
{
	iterateFiles([&](const std::shared_ptr<const File>& file)
	{
		if (file->dirty)
			saveFile(file->id);
		return true;
	});

	tinyxml2::XMLDocument doc;
	doSave(doc);
	doc.SaveFile(filename.c_str());
	m_filename = filename;
	m_isDirty = false;
	fireEvent(DataEventID::WorkspaceDirty, false);
	fireEvent(DataEventID::WorkspaceChanges, false);
}

void Workspace::close()
{
	// Remove all breakpoints
	while (m_breakpoints.getCount())
	{
		auto brk = m_breakpoints.getAt(m_breakpoints.getCount() - 1);
		removeBreakpoint(brk->file->id, brk->line);
	}

	// Remove all watches
	while (m_watches.size())
	{
		removeWatchByID(m_watches.front().id);
	}

	m_filename = "";

	// Remove all files
	m_files._clearAll();

	m_isDirty = false;
	fireEvent(DataEventID::WorkspaceChanges, false);
	fireEvent(DataEventID::WorkspaceDirty, false);
}

void Workspace::load(const UTF8String& filename)
{
	tinyxml2::XMLDocument doc;
	if (doc.LoadFile(filename.c_str()) != tinyxml2::XML_NO_ERROR)
		throw std::runtime_error("Error loading workspace file.");
	doLoad(doc);
	m_filename = filename;
	m_isDirty = false;
	fireEvent(DataEventID::WorkspaceDirty, false);
}


} // namespace nutcracker
