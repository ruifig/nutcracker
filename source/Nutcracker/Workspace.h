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

enum class DataEventID
{
	// Files
	FileDirty,
	FileSaved,
	// Breakpoints
	BreakpointFIRST,
	BreakpointAdd=BreakpointFIRST,
	BreakpointToggle,
	BreakpointDropChanges,
	BreakpointRemoved,
	BreakpointLAST = BreakpointRemoved
};

struct DataEvent
{
	explicit DataEvent(DataEventID id) : id(id){}
	DataEventID id;

	bool isBreakpointEvent() const
	{
		return id >= DataEventID::BreakpointFIRST && id <= DataEventID::BreakpointLAST;
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
	// Files
	//
	std::shared_ptr<const File> getFile(FileId fileId);
	// TODO : Rename this, as it doesn't create a file. It creates the object that tracks an existing file
	std::shared_ptr<const File> createFile(UTF8String path);
	void setFileSaveFunc(FileId fileId, std::function<bool(const std::shared_ptr<const File>& file)> func);
	void addFolder(const UTF8String& path);
	void resyncFolders();
	std::shared_ptr<const Folder> getRoot();
	void setFileDirty(FileId fileId, bool dirty);
	bool saveFile(FileId fileId);

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
private:

	const Breakpoint* toggleBreakpoint(Breakpoint* brk);
	void fireEvent(const DataEvent& evt);
	std::vector<std::pair<void*,std::function<void(const DataEvent&)>>> m_listeners;
	Files m_files;
	Breakpoints m_breakpoints;
	int m_inEventHandler = 0;
};

} // namespace nutcracker
