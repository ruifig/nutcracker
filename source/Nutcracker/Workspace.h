/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com
	
	purpose:
	
*********************************************************************/
#pragma once

#include "Files.h"
#include "Breakpoints.h"

namespace nutcracker
{

// Forward declarations
class DebugSession;
struct BreakInfo;
class Interpreter;


struct Options
{
	UTF8String general_defaultInterpreter;
	bool view_indentation = false;
	bool view_whitespaces = false;
	bool view_eol = false;
};

enum class DataEventID
{
	// Workspace
	WorkspaceChanges,
	WorkspaceDirty,

	// Files
	FileDirty,
	FileSaved,

	// Breakpoints
	BreakpointFIRST,
	BreakpointAdd=BreakpointFIRST,
	BreakpointToggle,
	BreakpointDropChanges,
	BreakpointUpdated,
	BreakpointValidated,
	BreakpointRemoved,
	BreakpointLAST = BreakpointRemoved,

	// Watches
	WatchesChanged,

	//
	DebugFIRST,
	DebugStart=DebugFIRST,
	DebugStop,
	DebugBreak,
	DebugChangedCallstackFrame,
	DebugResume,
	DebugLAST=DebugResume,

	// Options
	ViewFIRST,
	ViewIndentation=ViewFIRST,
	ViewWhitespaces,
	ViewEOL,
	ViewLAST = ViewEOL,

	// Interpreters
	InterpreterChanged

};

struct DataEvent
{
	explicit DataEvent(DataEventID id, bool makesWorkspaceDirty) : id(id), makesWorkspaceDirty(makesWorkspaceDirty)
	{
	}

	DataEventID id;
	bool makesWorkspaceDirty = false;

	bool isBreakpointEvent() const
	{
		return id >= DataEventID::BreakpointFIRST && id <= DataEventID::BreakpointLAST;
	}
	
	bool isViewOptionsEvent() const
	{
		return id >= DataEventID::ViewFIRST && id <= DataEventID::ViewLAST;
	}
	
	bool isDebugEvent() const
	{
		return id >= DataEventID::DebugFIRST && id <= DataEventID::DebugLAST;
	}

};

//////////////////////////////////////////////////////////////////////////
//	File events
//////////////////////////////////////////////////////////////////////////
struct FileEvent : public DataEvent
{
	FileEvent(DataEventID id, const std::shared_ptr<File>& file): DataEvent(id, false), file(file) {}
	std::shared_ptr<File> file;
};
struct FileDirty : public FileEvent
{
	explicit FileDirty(const std::shared_ptr<File>& file) : FileEvent(DataEventID::FileDirty, file){}
};
struct FileSaved : public FileEvent
{
	explicit FileSaved(const std::shared_ptr<File>& file) : FileEvent(DataEventID::FileSaved, file){}
};

//////////////////////////////////////////////////////////////////////////
//	Breakpoint events
//////////////////////////////////////////////////////////////////////////
struct BreakpointEvent : public DataEvent
{
	BreakpointEvent(DataEventID id, const Breakpoint* brk, bool makesWSDirty) : DataEvent(id, makesWSDirty), brk(brk){}
	const Breakpoint* brk;
};
struct BreakpointAdd : public BreakpointEvent
{
	BreakpointAdd(const Breakpoint* brk) : BreakpointEvent(DataEventID::BreakpointAdd, brk, true) {}
};
struct BreakpointToggle : public BreakpointEvent
{
	BreakpointToggle(const Breakpoint* brk) : BreakpointEvent(DataEventID::BreakpointToggle, brk, true) {}
};
struct BreakpointDropChanges : public BreakpointEvent
{
	BreakpointDropChanges(const Breakpoint* brk) : BreakpointEvent(DataEventID::BreakpointDropChanges, brk, false) {}
};
struct BreakpointRemoved : public BreakpointEvent
{
	BreakpointRemoved(const Breakpoint* brk) : BreakpointEvent(DataEventID::BreakpointRemoved, brk, true) {}
};
struct BreakpointUpdated : public BreakpointEvent
{
	BreakpointUpdated(const Breakpoint* brk) : BreakpointEvent(DataEventID::BreakpointUpdated, brk, true) {}
};
struct BreakpointValidated : public BreakpointEvent
{
	BreakpointValidated(const Breakpoint* brk) : BreakpointEvent(DataEventID::BreakpointValidated, brk, false) {}
};

struct Watch
{
	std::string name;
	int id;
};

//////////////////////////////////////////////////////////////////////////
//	Workspace
//////////////////////////////////////////////////////////////////////////
class Workspace
{
public:
	Workspace();
	Workspace(const Workspace&) = delete;

	cz::UTF8String getName() const;
	cz::UTF8String getFullPath() const;
	bool isDirty();

	void registerListener(void* listener, std::function<void(const DataEvent&)> func);
	void removeListener(void* listener);

	void save(const UTF8String& filename);
	void load(const UTF8String& filename);
	void close();

	//
	// Config
	//
	void loadConfig();
	void saveConfig();

	//
	// Files
	//
	std::shared_ptr<const File> getFile(FileId fileId);
	std::shared_ptr<const Folder> getFolder(FileId folderId);
	std::shared_ptr<const BaseItem> getItem(FileId itemId);

	// TODO : Rename this, as it doesn't create a file. It creates the object that tracks an existing file
	std::shared_ptr<const File> createFile(UTF8String path);
	void setFileSaveFunc(FileId fileId, std::function<bool(const std::shared_ptr<const File>& file)> func);
	void removeFileSaveFunc(FileId fileId);
	void addFolder(const UTF8String& path);
	void removeFolder(const UTF8String& path);
	void resyncFolders();
	std::shared_ptr<const Folder> getRoot();
	void setFileDirty(FileId fileId, bool dirty);
	bool saveFile(FileId fileId);
	void iterateFiles(std::function<bool(const std::shared_ptr<const File>&)>&& func);


	//
	// Breakpoints
	//
	int getBreakpointCount();
	const Breakpoint* addBreakpoint(FileId fileId, int line, int markerHandler, bool enabled=true);
	void iterateBreakpoints(FileId fileId , std::function<void(const Breakpoint* brk)> func);
	void iterateBreakpoints(std::function<void(const Breakpoint* brk)> func);
	const Breakpoint* getBreakpoint(int index);
	const Breakpoint* toggleBreakpoint(int index);
	const Breakpoint* toggleBreakpoint(FileId fileId, int line);
	void dropBreakpointChanges(FileId fileId);
	// This is used to update the breakpoint line when we are editing a file
	void setBreakpointPos(const Breakpoint* brk, int line, int markerHandler);
	void removeBreakpoint(int index);
	void removeBreakpoint(FileId fileId, int line);


	//
	// Watches
	//
	int addWatch(std::string watch, int id=0);
	int getWatchCount() const;
	void iterateWatches(std::function<void(const struct Watch*, const struct WatchValue*)> func);
	const Watch* getWatchByID(int id);
	void removeWatchByID(int id);

	//
	//
	//
	bool run(FileId fileId);

	//
	// Debugger
	//
	bool debuggerStart(FileId fileId);
	bool debuggerActive();
	const BreakInfo* debuggerGetBreakInfo();
	void debuggerSetCallstackFrame(int index);
	void debuggerResume();
	void debuggerStepOver();
	void debuggerStepInto();
	void debuggerStepReturn();
	void debuggerTerminate();
	void debuggerSuspend();

	//
	// View options
	//
	void setViewIdentation(bool enabled);
	void setViewWhitespaces(bool enabled);
	void setViewEOL(bool enabled);
	const Options* getViewOptions();


	//
	// Interpreters
	//
	void loadInterpreters();
	int getNumInterpreters();
	const Interpreter* getInterpreter(int index);
	const Interpreter* getCurrentInterpreter();
	void setCurrentInterpreter(int index);

protected:
	friend class DebugSession;
	void _iterateBreakpoints(std::function<void(Breakpoint* brk)> func);

private:

	Interpreter* _getCurrentInterpreter();

	void doSave(tinyxml2::XMLDocument& doc);
	void doLoad(tinyxml2::XMLDocument& doc);

	const Breakpoint* toggleBreakpoint(Breakpoint* brk);
	void fireEvent(const DataEvent& evt);
	void fireEvent(DataEventID id, bool makesWSDirty);
	void doDebugStop();
	void doDebugResume();

	void doRemoveBreakpoint(const std::unique_ptr<Breakpoint>& brk);
	cz::Filename m_filename;
	bool m_isDirty = false;

	std::vector<std::pair<void*,std::function<void(const DataEvent&)>>> m_listeners;
	std::vector<std::function<void()>> m_pendingFuncs;
	Files m_files;
	Breakpoints m_breakpoints;
	int m_inEventHandler = 0;
	std::shared_ptr<DebugSession> m_debugSession;
	std::shared_ptr<BreakInfo> m_breakInfo;
	Options m_options;
	std::vector<Watch> m_watches;
	int m_watchIDCounter = 1;

	//
	// Interpreters
	struct Interpreters
	{
		std::vector<std::unique_ptr<Interpreter>> all;
		int current=0; // index of the current interpreter
	} m_interpreters;

};

} // namespace nutcracker
