/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com

	created:	2012/02/27
	created:	27:2:2012   23:05
	filename: 	czFile.cpp
	
	purpose:
	
*********************************************************************/

#include "czPlatformStableHeaders.h"
#include "czFile.h"
#include "czFileSystem.h"
#include "czLogger.h"

namespace cz
{

File::File()
{
	mFile = NULL;
}

File::File(const UTF8String& filename, Mode mode)
{
	mFile = NULL;
	open(filename, mode);
}

File::File(const char* filename, Mode mode)
{
	mFile = NULL;
	open(filename, mode);
}

File::~File()
{
	close();
}


File::File(File&& other)
{
	mName = std::move(other.mName);
	mFile = std::move(other.mFile);
	mMode = std::move(other.mMode);
	other.mFile = nullptr;
}

File& File::operator=(File&& other)
{
	close();
	mName = std::move(other.mName);
	mFile = std::move(other.mFile);
	mMode = std::move(other.mMode);
	other.mFile = nullptr;
	return *this;
}

void File::close()
{
	if (mFile)
	{
		fclose(mFile);
		mFile = nullptr;
	}
}

void File::open(const UTF8String& filename, Mode mode)
{
	_open(filename, mode, true);
}

void File::open(const char* filename, Mode mode)
{
	_open(UTF8String(filename), mode, true);
}


bool File::try_open(const UTF8String& filename, Mode mode)
{
	return _open(filename, mode, false);
}

bool File::try_open(const char* filename, Mode mode)
{
	return _open(UTF8String(filename), mode, false);
}

bool File::_open(const UTF8String& filename, Mode mode, bool raiseError)
{
	mName = filename;
	if (mName.isRelativePath())
		mName = Filesystem::getSingleton().getCWD() + mName;

	CZ_ASSERT(mFile==NULL);
	wchar_t fmodebuf[4];
	wchar_t* fmode = &fmodebuf[0];
	if (mode==FILEMODE_READ)
		*fmode = 'r';
	else if (mode==FILEMODE_WRITE)
		*fmode = 'w';
	else if (mode==FILEMODE_READWRITE_EXISTING)
	{
		// Opens for read write, but the file needs to exist
		*(fmode++) = 'r';
		*fmode = '+';
	}
	else if (mode == FILEMODE_APPEND)
		*fmode = 'a';
	else if (mode==FILEMODE_APPENDNEW)
	{
		*(fmode++) = 'w';
		*fmode = '+';
	}
	else
	{
		CZ_ASSERT_F(0, "Invalid file open mode <%d>", static_cast<int>(mode));
		return false;
	}

	*(++fmode) ='b';
	(++fmode)[0] = 0;

	mFile = _wfopen(filename.widen().c_str(), fmodebuf);
	if (mFile==NULL)
	{
		if (raiseError)
			czError(ID_Log, "Couldn't open file <%s>, with mode <%d>.", filename, static_cast<int>(mode));
		return false;
	}
	mMode = mode;
	return (mFile) ? true : false;
}

bool File::isOpen() const
{
	return (mFile) ? true : false;
}

size_t File::write(const void* buffer, int size, int count)
{
	CZ_ASSERT(mFile);
	CZ_ASSERT(mMode==FILEMODE_WRITE || mMode==FILEMODE_READWRITE_EXISTING || mMode==FILEMODE_APPEND || mMode==FILEMODE_APPENDNEW);
	size_t r = fwrite(buffer, size, count, mFile);
	CZ_ASSERT_F(r==count, "%s failed. Requested %d elements (%d bytes each), and did %d", __FUNCTION__, static_cast<int>(count), static_cast<int>(size), static_cast<int>(r));
	return r;
}

size_t File::read(void* buffer, int size, int count)
{
	CZ_ASSERT(mFile);
	CZ_ASSERT(mMode==FILEMODE_READ || mMode==FILEMODE_READWRITE_EXISTING || mMode==FILEMODE_APPEND || mMode==FILEMODE_APPENDNEW);
	size_t r = fread(buffer, size, count, mFile);
	CZ_ASSERT_F((size*count==0) || r==count, "%s failed. Requested %d elements (%d bytes each), and did %d", __FUNCTION__, static_cast<int>(count), static_cast<int>(size), static_cast<int>(r));
	return r;
}

size_t File::write(const void* buffer, int bytes)
{
	return write(buffer, bytes, 1);
}

size_t File::read(void* buffer, int bytes)
{
	return read(buffer, bytes, 1);
}

bool File::eof() const
{
	CZ_ASSERT(mFile);
	return (feof(mFile)==0) ? false : true;
}

int File::tell() const
{
	CZ_ASSERT(mFile);
	return ftell(mFile);
}

bool File::seek(int offset, SeekMode seekmode)
{
	int smode = SEEK_SET;
	if (seekmode==FILESEEK_CURRENT)
		smode = SEEK_CUR;
	else if (seekmode==FILESEEK_END)
		smode = SEEK_END;

	CZ_ASSERT(mFile);
	int r = fseek(mFile, offset, smode);
	CZ_ASSERT(r==0);
	return (r==0) ? true : false;
}

int File::size()
{
	int originalPos = tell();
	seek(0, FILESEEK_END);
	int size = tell();
	seek(originalPos, FILESEEK_SET);
	return size;
}


} // namespace cz
