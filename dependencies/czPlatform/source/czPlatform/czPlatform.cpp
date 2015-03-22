//
// CrazyGaze (http://www.crazygaze.com)
// Author : Rui Figueira
// Email  : rui@crazygaze.com
//
// --------------------------------------------------------------
//
//

#include "czPlatformStableHeaders.h"
#include "czString.h"

#if defined(_MSC_VER)
extern "C" {
	_CRTIMP void __cdecl _wassert(_In_z_ const wchar_t * _Message, _In_z_ const wchar_t *_File, _In_ unsigned _Line);
}
#endif


namespace cz
{

namespace
{
	void PlatformCompileTimeAsserts()
	{
		CZ_COMPILETIME_ASSERT(sizeof(s8)==1);
		CZ_COMPILETIME_ASSERT(sizeof(u8)==1);
		CZ_COMPILETIME_ASSERT(sizeof(s16)==2);
		CZ_COMPILETIME_ASSERT(sizeof(u16)==2);
		CZ_COMPILETIME_ASSERT(sizeof(s32)==4);
		CZ_COMPILETIME_ASSERT(sizeof(u32)==4);
		CZ_COMPILETIME_ASSERT(sizeof(s64)==8);
		CZ_COMPILETIME_ASSERT(sizeof(u64)==8);
	}
}

void _doAssert(const char* file, int line, const char* fmt, ...)
{
	char buf1[1024];
    char buf2[1024];
	va_list args;
	va_start(args, fmt);
	cz_vsnprintf(buf1, 1024, fmt, args);
	va_end(args);
	cz_snprintf(buf2, 1024, "%s:%d: %s\n", file, line, buf1);

	// TODO : Add some kind of cross-platform logging

#if CZ_PLATFORM == CZ_PLATFORM_WIN32
    #if defined(_MSC_VER)
	    wchar_t wbuf[1024];
	    wchar_t wfile[1024];

	    mbstowcs(wbuf, buf1, 1024);
	    mbstowcs(wfile, file, 1024);

	    _wassert(wbuf, wfile, line);
	    __debugbreak(); // This will break in all builds
    #elif defined(__MINGW32__)
	    DebugBreak();
    #endif
#elif CZ_PLATFORM == CZ_PLATFORM_SYMBIAN
	printf(buf2);
    __ASSERT_ALWAYS(0,0);
#else
    #error debugbreak not implemented for current platform/compiler
#endif
}

#if CZ_PLATFORM==CZ_PLATFORM_WIN32
const char* getLastWin32ErrorMessage(int err)
{
	DWORD errCode = (err==0) ? GetLastError() : err;
	char* errString = cz::getTemporaryString();
	// http://msdn.microsoft.com/en-us/library/ms679351(VS.85).aspx
	int size = FormatMessageA(
		FORMAT_MESSAGE_FROM_SYSTEM, // use windows internal message table
		0,       // 0 since source is internal message table
		errCode, // this is the error code returned by WSAGetLastError()
		// Could just as well have been an error code from generic
		// Windows errors from GetLastError()
		0,       // auto-determine language to use
		errString, // this is WHERE we want FormatMessage
		CZ_TEMPORARY_STRING_MAX_SIZE,
		0 );               // 0, since getting message from system tables
	return errString;
}
#endif


}

