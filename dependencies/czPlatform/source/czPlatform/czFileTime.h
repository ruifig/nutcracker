/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com
	
	purpose:
	Encapsulates a file time, and provides some basic operations
	
*********************************************************************/

#pragma once
#ifndef _czFileTime_h_
#define _czFileTime_h_

#include "czPlatformPrerequisites.h"
#include "czUTF8String.h"

#if CZ_PLATFORM==CZ_PLATFORM_WIN32
	#include <windows.h>
#endif

namespace cz
{

class FileTime
{
public:
	FileTime();
#if CZ_PLATFORM==CZ_PLATFORM_WIN32
	FileTime(FILETIME time);
#endif

	bool isValid() const;

	bool operator==(const FileTime& other) const;
	bool operator<(const FileTime& other) const;
	bool operator<=(const FileTime& other) const;
	bool operator>(const FileTime& other) const;
	bool operator>=(const FileTime& other) const;
	bool operator!=(const FileTime& other) const;

	bool youngerThan(const FileTime& other) const;
	bool olderThan(const FileTime& other) const;


	enum TimeType
	{
		kCreated,
		kModified
	};

	static FileTime get(const UTF8String& filename, TimeType type);

public:
#if CZ_PLATFORM==CZ_PLATFORM_WIN32
	FILETIME m_time;
#endif
};


} // namespace cz

#endif // _czFileTime_h_
