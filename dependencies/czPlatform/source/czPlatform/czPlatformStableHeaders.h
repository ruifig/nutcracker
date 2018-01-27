/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com

	created:	2012/02/26
	created:	26:2:2012   1:02
	filename: 	czPlatformStableHeaders.h
	
	purpose:
	For use with compilers that support precompiled headers.
	Files that rarely change should go here.
*********************************************************************/

#ifndef czPlatformStableHeaders_h__
#define czPlatformStableHeaders_h__

// Include this first, so we have all the defines we need available
#include "czPlatform.h"

// platform dependent headers
#if CZ_PLATFORM==CZ_PLATFORM_WIN32

	//#include <SDKDDKVer.h>

	#ifndef WIN32_LEAN_AND_MEAN
		//#  define WIN32_LEAN_AND_MEAN
	#endif
	#if !defined(NOMINMAX) && defined(_MSC_VER)
	#	define NOMINMAX // required to stop windows defines from messing std::max,min, std::numeric_limits::max, etc
	#endif
	#include "winsock2.h"
	#include <windows.h>
	#include <process.h>
	#include <Psapi.h>
#elif CZ_PLATFORM == CZ_PLATFORM_SYMBIAN
	#include <e32base.h>
	#include <e32std.h> 
	#include <e32debug.h>
	#include <f32file.h>
	#include <utf.h>
    #include <glib.h>
#endif

// C headers
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

// C++ headers
#include <algorithm>
#include <limits>
#include <iterator>
#include <utility>
#include <map>
#include <vector>
#include <string>
#include <queue>
#include <cstdint>

// czPlatform Headers
#include "czSingleton.h"
#include "czPlatformRoot.h"
#include "czStandardC.h"

#endif // czPlatformStableHeaders_h__
