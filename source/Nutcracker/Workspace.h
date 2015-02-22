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

struct Workspace 
{
	Workspace()
	{
	}

	~Workspace()
	{
	}

	Files files;
	Breakpoints breakpoints;
};

} // namespace nutcracker
