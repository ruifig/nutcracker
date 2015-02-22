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
	std::shared_ptr<File> file;
	int markerHandle = -1;  // From this handle, we can get the line
	int line = -1;
	int savedline = -1; // Line this breakpoint was at the last time the file was saved
	bool enabled = false;
	Breakpoint(const std::shared_ptr<File>& file, int line, int markerHandle, bool enabled);
};

class Breakpoints
{
public:
	Breakpoints();
	void add(const std::shared_ptr<File>& file, int line, int markerHandle, bool enabled=true);
	void remove(const std::shared_ptr<File>& file, int line);

	int getCount() const;
	void iterate(const std::shared_ptr<File>& file, std::function<void(Breakpoint& brk)>&& func);
	void iterate(std::function<void(Breakpoint& brk)>&& func);
	Breakpoint& getAt(int index);

	Breakpoint* get(const std::shared_ptr<File>& file, int line);
private:
	//std::vector<Breakpoint>& getAll();
	std::vector<Breakpoint> m_all;
};

} // namespace nutcracker
