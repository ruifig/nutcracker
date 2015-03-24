/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com

	created:	2012/03/09
	created:	9:3:2012   0:56
	filename: 	czFileSystem.cpp

	purpose:

*********************************************************************/

#include "czPlatformStableHeaders.h"
#include "czFileSystem.h"
#include "Shlwapi.h"
#include "Shlobj.h"
#include <strsafe.h>
#include "czPlatform/czLogger.h"

namespace cz
{


CZ_DEFINE_SINGLETON(Filesystem)

// Per-platform helper code
namespace
{
#if CZ_PLATFORM == CZ_PLATFORM_WIN32
	void _getCurrentDirectory(Filename& dest)
	{
		const int bufferLength = MAX_PATH;
		wchar_t buf[bufferLength+1];
		buf[0] = 0;
		CZ_CHECK(GetCurrentDirectoryW(bufferLength, buf)!=0);
		dest = Filename(std::wstring(buf));
		dest += '\\';
	}

	void _getAndSetDefaultCWD(Filename& dest)
	{
		const int bufferLength = MAX_PATH;
		wchar_t buf[bufferLength+1];
		// Change Current directory to where the executable is
		CZ_CHECK( GetModuleFileNameExW(GetCurrentProcess(), NULL, buf, bufferLength) !=0 );
		Filename f = &buf[0];
		CZ_CHECK( SetCurrentDirectoryW(f.getDirectory().widen().c_str()) !=0 );
		CZ_CHECK( GetCurrentDirectoryW(bufferLength, buf)!=0 );
		dest = Filename(std::wstring(buf));
		dest += '\\';
	}

	void _getExecutableDirectory(Filename& dest)
	{
		const int bufferLength = MAX_PATH;
		wchar_t buf[bufferLength+1];
		// Change Current directory to where the executable is
		CZ_CHECK( GetModuleFileNameExW(GetCurrentProcess(), NULL, buf, bufferLength) !=0 );
		dest = Filename(std::wstring(buf)).getDirectory();
	}

	bool _isExistingDirectory(const cz::UTF8String& path)
	{
		return PathIsDirectoryW(path.widen().c_str()) ? true : false;
	}

	bool _isExistingFile(const cz::UTF8String& filename)
	{
		DWORD dwAttrib = GetFileAttributesW(filename.widen().c_str());

		return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
			!(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
	}

	bool _createDirectory(const cz::UTF8String& path)
	{
		return SHCreateDirectoryExW(NULL, path.widen().c_str(), NULL)==ERROR_SUCCESS ? true : false;
	}

#else
	#error Not implemented
#endif

}

Filesystem::Filesystem()
{
	_getCurrentDirectory(mOriginalCWD);
	mCWD = mOriginalCWD;
}

Filesystem::~Filesystem()
{
}

const Filename& Filesystem::getCWD() const
{
	return mCWD;
}

const Filename& Filesystem::getOriginalCWD() const
{
	return mOriginalCWD;
}

Filename Filesystem::getExecutableDirectory()
{
	Filename dir;
	Filename canonicalized;
	_getExecutableDirectory(dir);
	fullPath(canonicalized, dir);
	return canonicalized.size() ? canonicalized : dir;
}

void Filesystem::setCWD(const UTF8String& path)
{
	mCWD = path;
}

void Filesystem::setCWDToDefault()
{
	_getAndSetDefaultCWD(mCWD);
}

bool Filesystem::isExistingDirectory(const UTF8String& path)
{
	return _isExistingDirectory(path);
}

bool Filesystem::createRelativePath(UTF8String& dst, const UTF8String& from, const UTF8String& to)
{
	wchar_t szOut[MAX_PATH+1] = L"";

	if (!PathRelativePathToW(
			szOut,
			from.widen().c_str(), FILE_ATTRIBUTE_DIRECTORY,
			to.widen().c_str(), FILE_ATTRIBUTE_NORMAL))
		return false;

	dst = szOut;
	return true;
}

bool Filesystem::isExistingFile(const UTF8String& filename)
{
	return _isExistingFile(filename);
}

bool Filesystem::createDirectory(const UTF8String& path)
{
	return _createDirectory(path);
}

bool Filesystem::fullPath(UTF8String& dst, const UTF8String& path, const UTF8String& root)
{
	wchar_t fullpathbuf[MAX_PATH+1];
	wchar_t srcfullpath[MAX_PATH+1];
	UTF8String tmp = isRelativePath(path) ? root+path : path;
	wcscpy(srcfullpath, tmp.widen().c_str());
	wchar_t* d = srcfullpath;
	wchar_t* s = srcfullpath;
	while(*s)
	{
		if (*s == '/')
			*s = '\\';
		*d++= *s;

		// Skip any repeated separator
		if (*s == '\\')
		{
			s++;
			while (*s && (*s == '\\' || *s == '/'))
				s++;
		}
		else
		{
			s++;
		}
	}
	*d = 0;

	bool res = PathCanonicalizeW(fullpathbuf, srcfullpath) ? true : false;
	if (res)
		dst = fullpathbuf;
	return res;
}

UTF8String Filesystem::pathStrip(const UTF8String& path)
{
	wchar_t srcfullpath[MAX_PATH+1];
	wcscpy(srcfullpath, path.widen().c_str());
	wchar_t* p = srcfullpath;
	PathStripPath(p);
	return UTF8String(p);
}

bool Filesystem::isRelativePath(const UTF8String& path)
{
	return PathIsRelativeW(path.widen().c_str()) ? true : false;
}


// Copied from  http://msdn.microsoft.com/en-gb/library/windows/desktop/aa365200(v=vs.85).aspx
std::vector<Filesystem::FileInfo> Filesystem::getFilesInDirectory(const UTF8String& path, const UTF8String& wildcard, bool includeDirectories)
{
	std::vector<FileInfo> res;

	// Check that the input path plus 3 is not longer than MAX_PATH.
	// Three characters are for the "\*" plus NULL appended below.

	size_t length_of_arg;
	TCHAR szDir[MAX_PATH];
	WIN32_FIND_DATA ffd;

	StringCchLengthW(path.widen().c_str(), MAX_PATH, &length_of_arg);

	if (length_of_arg > (MAX_PATH - 3))
	{
		czWarning(ID_Log, "Directory path is too long");
		return res;
	}

	// Prepare string for use with FindFile functions.  First, copy the
	// string to a buffer, then append '\*' to the directory name.
	StringCchCopy(szDir, MAX_PATH, path.widen().c_str());
	StringCchCat(szDir, MAX_PATH, (UTF8String("\\")+wildcard).widen().c_str());

	// Find the first file in the directory.
	auto hFind = FindFirstFile(szDir, &ffd);

	if (INVALID_HANDLE_VALUE == hFind)
	{ 
		czWarning(ID_Log, "%s failed: %s", __FUNCTION__, getLastWin32ErrorMessage());
		return res;
	}

	// List all the files in the directory with some info about them.

	do
	{
		if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (includeDirectories &&
				(StrCmp(ffd.cFileName, TEXT("."))!=0) &&
				(StrCmp(ffd.cFileName, TEXT(".."))!=0))
			{
				res.push_back({ Type::Directory, Filename(ffd.cFileName), 0 });
			}
		}
		else
		{
			LARGE_INTEGER filesize;
			filesize.LowPart = ffd.nFileSizeLow;
			filesize.HighPart = ffd.nFileSizeHigh;
			res.push_back({ Type::File, Filename(ffd.cFileName), filesize.QuadPart });
		}
	} while (FindNextFile(hFind, &ffd) != 0);


	return res;
}

} // namespace cz

