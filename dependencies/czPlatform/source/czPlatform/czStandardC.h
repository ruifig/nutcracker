/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com

	created:	2012/03/09
	created:	9:3:2012   0:17
	filename: 	czStandardC.h
	
	purpose:
	Implementation of some C functions, which are missing in some platforms
*********************************************************************/

#ifndef czStandardC_h__
#define czStandardC_h__

#include "czPlatformPrerequisites.h"
#include <stdarg.h>

/*! Same as snprintf */
int cz_snprintf(char* buffer, int bufSize, const char* format, ...);
/*! Same as snprintf */
int cz_vsnprintf(char* buffer, int bufSize, const char* format, va_list args);

#endif // czStandardC_h__
