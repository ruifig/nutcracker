/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com

	created:	2012/03/09
	created:	9:3:2012   0:23
	filename: 	czStandardC.cpp
	
	purpose:
	Implementation of some C functions, which are missing in some platforms	
*********************************************************************/

#include "czPlatformStableHeaders.h"
#include "czStandardC.h"

int cz_snprintf(char* buffer, int bufSize, const char* format, ...)
{
	va_list args;
	va_start(args, format);
	return cz_vsnprintf(buffer, bufSize, format, args);
}

int cz_vsnprintf(char* buffer, int bufSize, const char* format, va_list args)
{
#if CZ_PLATFORM == CZ_PLATFORM_WIN32
	return _vsnprintf(buffer, bufSize, format, args);
#elif CZ_PLATFORM == CZ_PLATFORM_SYMBIAN
	return vsnprintf(buffer, bufSize, format, args);
#else
	#error Implementation missing
#endif
}

