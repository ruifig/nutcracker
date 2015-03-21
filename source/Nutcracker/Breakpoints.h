/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com
	
	purpose:
	
*********************************************************************/
#pragma once

#include "Files.h"

namespace nutcracker
{

struct Breakpoint
{
	std::shared_ptr<const File> file;
	int markerHandle = -1;  // From this handle, we can get the line
	int line = -1;
	int savedline = -1; // Line this breakpoint was at the last time the file was saved
	bool enabled = false;
	bool valid = false;
	Breakpoint(const std::shared_ptr<const File>& file, int line, int markerHandle, bool enabled);
};

class Breakpoints
{
public:
	explicit Breakpoints(Workspace* outer);
	Breakpoint* add(const std::shared_ptr<const File>& file, int line, int markerHandle, bool enabled=true);
	std::unique_ptr<Breakpoint> remove(const std::shared_ptr<const File>& file, int line);
	std::unique_ptr<Breakpoint> removeAt(int index);

	int getCount() const;
	void iterate(const std::shared_ptr<const File>& file, std::function<void(Breakpoint* brk)>&& func);
	void iterate(std::function<void(Breakpoint* brk)>&& func);
	Breakpoint* getAt(int index);
	Breakpoint* get(const std::shared_ptr<const File>& file, int line);
	void _clearAll();
private:

	Workspace* m_outer;
	std::vector<std::unique_ptr<Breakpoint>> m_all;
};

} // namespace nutcracker
