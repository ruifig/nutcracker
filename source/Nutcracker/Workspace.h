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
	explicit DataEvent(DataEventID id) : id(id){}
	DataEventID id;

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
	FileEvent(DataEventID id, const std::shared_ptr<File>& file) : DataEvent(id), file(file) {}
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
	BreakpointEvent(DataEventID id, const Breakpoint* brk) : DataEvent(id), brk(brk){}
	const Breakpoint* brk;
};
struct BreakpointAdd : public BreakpointEvent
{
	BreakpointAdd(const Breakpoint* brk) : BreakpointEvent(DataEventID::BreakpointAdd, brk) {}
};
struct BreakpointToggle : public BreakpointEvent
{
	BreakpointToggle(const Breakpoint* brk) : BreakpointEvent(DataEventID::BreakpointToggle, brk) {}
};
struct BreakpointDropChanges : public BreakpointEvent
{
	BreakpointDropChanges(const Breakpoint* brk) : BreakpointEvent(DataEventID::BreakpointDropChanges, brk) {}
};
struct BreakpointRemoved : public BreakpointEvent
{
	BreakpointRemoved(const Breakpoint* brk) : BreakpointEvent(DataEventID::BreakpointRemoved, brk) {}
};
struct BreakpointUpdated : public BreakpointEvent
{
	BreakpointUpdated(const Breakpoint* brk) : BreakpointEvent(DataEventID::BreakpointUpdated, brk) {}
};
struct BreakpointValidated : public BreakpointEvent
{
	BreakpointValidated(const Breakpoint* brk) : BreakpointEvent(DataEventID::BreakpointValidated, brk) {}
};


//////////////////////////////////////////////////////////////////////////
//	Debugger events
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//	Workspace
//////////////////////////////////////////////////////////////////////////
class Workspace
{
public:
	Workspace();
	Workspace(const Workspace&) = delete;

	void registerListener(void* listener, std::function<void(const DataEvent&)> func);
	void removeListener(void* listener);


	//
	// Config
	//
	void loadConfig();
	void saveConfig();

	//
	// Files
	//
	std::shared_ptr<const File> getFile(FileId fileId);
	// TODO : Rename this, as it doesn't create a file. It creates the object that tracks an existing file
	std::shared_ptr<const File> createFile(UTF8String path);
	void setFileSaveFunc(FileId fileId, std::function<bool(const std::shared_ptr<const File>& file)> func);
	void removeFileSaveFunc(FileId fileId);
	void addFolder(const UTF8String& path);
	void resyncFolders();
	std::shared_ptr<const Folder> getRoot();
	void setFileDirty(FileId fileId, bool dirty);
	bool saveFile(FileId fileId);
	void iterateFiles(std::function<void(const std::shared_ptr<const File>&)>&& func);


	//
	// Breakpoints
	//
	int getBreakpointCount();
	const Breakpoint* addBreakpoint(FileId fileId, int line, int markerHandler);
	void iterateBreakpoints(FileId fileId , std::function<void(const Breakpoint* brk)> func);
	void iterateBreakpoints(std::function<void(const Breakpoint* brk)> func);
	const Breakpoint* getBreakpoint(int index);
	const Breakpoint* toggleBreakpoint(int index);
	const Breakpoint* toggleBreakpoint(FileId fileId, int line);
	void dropBreakpointChanges(FileId fileId);
	// This is used to update the breakpoint line when we are editing a file
	void setBreakpointPos(const Breakpoint* brk, int line, int markerHandler);
	void removeBreakpoint(FileId fileId, int line);

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

	const Breakpoint* toggleBreakpoint(Breakpoint* brk);
	void fireEvent(const DataEvent& evt);
	void fireEvent(DataEventID id);
	void doDebugStop();
	void doDebugResume();
	std::vector<std::pair<void*,std::function<void(const DataEvent&)>>> m_listeners;
	std::vector<std::function<void()>> m_pendingFuncs;
	Files m_files;
	Breakpoints m_breakpoints;
	int m_inEventHandler = 0;
	std::shared_ptr<DebugSession> m_debugSession;
	std::shared_ptr<BreakInfo> m_breakInfo;
	Options m_options;


	//
	// Interpreters
	struct Interpreters
	{
		std::vector<std::unique_ptr<Interpreter>> all;
		int current=0; // index of the current interpreter
	} m_interpreters;

};

} // namespace nutcracker
