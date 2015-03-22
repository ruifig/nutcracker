/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com

	created:	2012/03/10
	created:	10:3:2012   1:18
	filename: 	czAtomicCount.h
	
	purpose:
	Based on boost atomic_count
*********************************************************************/


#ifndef czAtomicCount_h__
#define czAtomicCount_h__

#if CZ_PLATFORM == CZ_PLATFORM_WIN32
	#include "detail/czAtomicCount_win32.h"
#elif CZ_PLATFORM == CZ_PLATFORM_SYMBIAN
    #include "detail/czAtomicCount_glib.h"
#else
	#error Not implemented
#endif

#endif // czAtomicCount_h__


