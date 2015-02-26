/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com
	
	purpose:
	
*********************************************************************/

#include "NutcrackerPCH.h"
#include "Workspace.h"

namespace nutcracker
{

Workspace::Workspace() : m_files(this), m_breakpoints(this)
{
}

void Workspace::registerListener(void* listener, std::function<void(const DataEvent&)> func)
{
	CZ_ASSERT(m_inEventHandler == 0);
	m_listeners.emplace_back(listener, std::move(func));
}

void Workspace::removeListener(void* listener)
{
	CZ_ASSERT(m_inEventHandler == 0);
	cz::erase_if(m_listeners, [&](decltype(m_listeners[0])& i)
	{
		return i.first == listener;
	});
}

void Workspace::fireEvent(const DataEvent& evt)
{
	m_inEventHandler++;
	/*
	auto todo = std::move(m_listeners);
	for (auto& i : todo)
		i.second(evt);
	m_listeners.insert(m_listeners.begin(), todo.begin(), todo.end());
	*/
	for (auto& i : m_listeners)
		i.second(evt);
	m_inEventHandler--;
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

bool Workspace::saveFile(FileId fileId)
{
	auto file = m_files.getFile(fileId);
	CZ_ASSERT(file);
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

} // namespace nutcracker
