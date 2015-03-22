/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com
	
	purpose:
	Encapsulates a file time, and provides some basic operations
	
*********************************************************************/

#include "czPlatformStableHeaders.h"
#include "czFileTime.h"

namespace cz
{

FileTime::FileTime()
{
	memset(&m_time, 0, sizeof(m_time));
}

#if CZ_PLATFORM==CZ_PLATFORM_WIN32
FileTime::FileTime(FILETIME time) : m_time(time)
{
}
#endif

bool FileTime::isValid() const
{
	return !(m_time.dwHighDateTime==0 && m_time.dwLowDateTime==0);
}

bool FileTime::operator==(const FileTime& other) const
{
	return CompareFileTime(&m_time,&other.m_time)==0 ? true : false;
}

bool FileTime::operator<(const FileTime& other) const
{
	return CompareFileTime(&m_time,&other.m_time)<0 ? true : false;
}

bool FileTime::operator<=(const FileTime& other) const
{
	return CompareFileTime(&m_time,&other.m_time)<=0 ? true : false;
}

bool FileTime::operator>(const FileTime& other) const
{
	return CompareFileTime(&m_time,&other.m_time)>0 ? true : false;
}

bool FileTime::operator>=(const FileTime& other) const
{
	return CompareFileTime(&m_time,&other.m_time)>=0 ? true : false;
}

bool FileTime::operator!=(const FileTime& other) const
{
	return CompareFileTime(&m_time,&other.m_time)!=0 ? true : false;
}

bool FileTime::youngerThan(const FileTime& other) const
{
	if ( !(this->isValid() && other.isValid()) )
		return false;
	else
		return *this > other;
}

bool FileTime::olderThan(const FileTime& other) const
{
	if ( !(this->isValid() && other.isValid()) )
		return false;
	else
		return *this < other;
}

FileTime FileTime::get(const UTF8String& filename, FileTime::TimeType type)
{
	HANDLE hFile = CreateFileW(filename.widen().c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	FileTime filetime;
	if (hFile == INVALID_HANDLE_VALUE)
		return FileTime();

	FILETIME ftCreate, ftAccess, ftWrite;

	// Retrieve the file times for the file.
	if (GetFileTime(hFile, &ftCreate, &ftAccess, &ftWrite))
	{
		if (type==kCreated)
			filetime = FileTime(ftCreate);
		else if (type==kModified)
			filetime = FileTime(ftWrite);
		else
		{
			CZ_UNEXPECTED();
		}
	}

	CloseHandle(hFile);
	return filetime;
}

} // namespace cz

