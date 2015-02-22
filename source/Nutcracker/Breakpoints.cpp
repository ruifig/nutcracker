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
Breakpoint::Breakpoint(const std::shared_ptr<File>& file, int line, int markerHandle, bool enabled)
	: file(file), line(line), markerHandle(markerHandle), enabled(enabled)
{
	savedline = line;
}

Breakpoints::Breakpoints()
{
}

//////////////////////////////////////////////////////////////////////////
// Breakpoints
//////////////////////////////////////////////////////////////////////////

void Breakpoints::add(const std::shared_ptr<File>& file, int line, int markerHandle, bool enabled)
{
	for (auto& b : m_all)
	{
		if (b.file == file && b.markerHandle == markerHandle)
		{
			czDebug(ID_Log, "Breakpoint '%s:%d' already exists", file->name.c_str(), markerHandle);
			return;
		}
	}

	m_all.emplace_back(file, line, markerHandle, enabled);
}

void Breakpoints::remove(const std::shared_ptr<File>& file, int line)
{
	cz::erase_if(m_all, 
		[&](Breakpoint& b)
	{
		return b.file == file && b.line == line;
	});
}

void Breakpoints::iterate(const std::shared_ptr<File>& file, std::function<void(Breakpoint& brk)>&& func)
{
	for (auto& b : m_all)
	{
		if (b.file == file)
			func(b);
	}
}

Breakpoint* Breakpoints::get(const std::shared_ptr<File>& file, int line)
{
	for (auto& b : m_all)
	{
		if (b.file == file)
			return &b;
	}
	return nullptr;
}

std::vector<Breakpoint>& Breakpoints::getAll()
{
	return m_all;
}



} // namespace nutcracker
