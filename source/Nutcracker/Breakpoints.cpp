/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com
	
	purpose:
	
*********************************************************************/

#include "NutcrackerPCH.h"
#include "Breakpoints.h"

namespace nutcracker
{

//////////////////////////////////////////////////////////////////////////
// Breakpoint
//////////////////////////////////////////////////////////////////////////
Breakpoint::Breakpoint(const std::shared_ptr<const File>& file, int line, int markerHandle, bool enabled)
	: file(file), line(line), markerHandle(markerHandle), enabled(enabled)
{
	savedline = line;
}

//////////////////////////////////////////////////////////////////////////
// Breakpoints
//////////////////////////////////////////////////////////////////////////

Breakpoints::Breakpoints(Workspace* outer) : m_outer(outer)
{
}

Breakpoint* Breakpoints::add(const std::shared_ptr<const File>& file, int line, int markerHandle, bool enabled)
{
	for (auto& b : m_all)
	{
		if (b->file == file && b->markerHandle == markerHandle)
		{
			czDebug(ID_Log, "Breakpoint '%s:%d' already exists", file->name.c_str(), markerHandle);
			return nullptr;
		}
	}

	m_all.push_back(std::make_unique<Breakpoint>(file, line, markerHandle, enabled));
	return m_all.back().get();
}

std::unique_ptr<Breakpoint> Breakpoints::remove(const std::shared_ptr<const File>& file, int line)
{
	std::unique_ptr<Breakpoint> brk;
	cz::erase_if(m_all, 
		[&](std::unique_ptr<Breakpoint>& b)
	{
		if (b->file == file && b->line == line)
		{
			brk = std::move(b);
			return true;
		}
		else
		{
			return false;
		}
	});

	return brk;
}

void Breakpoints::iterate(const std::shared_ptr<const File>& file, std::function<void(Breakpoint* brk)>&& func)
{
	for (auto& b : m_all)
	{
		if (b->file == file)
			func(b.get());
	}
}

void Breakpoints::iterate(std::function<void(Breakpoint * brk)>&& func)
{
	for (auto& b : m_all)
	{
		func(b.get());
	}
}

Breakpoint* Breakpoints::get(const std::shared_ptr<const File>& file, int line)
{
	for (auto& b : m_all)
	{
		if (b->file == file)
			return b.get();
	}
	return nullptr;
}

int Breakpoints::getCount() const
{
	return static_cast<int>(m_all.size());
}

Breakpoint * Breakpoints::getAt(int index)
{
	CZ_ASSERT(unsigned(index) < m_all.size());
	return m_all[index].get();
}


} // namespace nutcracker
