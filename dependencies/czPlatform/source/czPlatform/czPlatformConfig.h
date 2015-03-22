/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com

	created:	2012/02/26
	created:	26:2:2012   1:39
	filename: 	czPlatformConfig.h

	purpose:

*********************************************************************/
//
/** @file */

// This file is included from czPlatform.h to configure the library
// You can set several defines per platform and build type, to configure the library as you see fit

#ifndef CZ_PLATFORM_CONFIG_H_
#define CZ_PLATFORM_CONFIG_H_

/**
\addtogroup BuildTweaks Tweaking the build
\brief
It's possible to tweak czPlatform building to fit your needs by editing the file config/czPlatformConfigCustom.h .
When changing that file, you can check several defines already available to help you tweak czPlatform according to
your needs and platform you're building for.
@{
*/

/** \name Platform Defines
These defines are defined by czPlatform itself, depending on what platform/architecture was detected during the building process.
You can use those to tweak the building per-platform.

They should be checked for value, not with defined(XX).
Example:
\code
#if CZ_PLATFORM==CZ_PLATFORM_WIN32
// do something
#endif
\endcode

	@{
*/

/**
\def CZ_PLATFORM
@hideinitializer
\brief Defined to the platform we're building for (CZ_PLATFORM_WIN32, CZ_PLATFORM_IOS, etc).
*/

#define CZ_PLATFORM_WIN32 1 /**< @hideinitializer \brief CZ_PLATFORM will have this value if we're building for Windows */
#define CZ_PLATFORM_BADA 2 /**< @hideinitializer \brief CZ_PLATFORM will have this value if we're building for BADA OS */
#define CZ_PLATFORM_IOS 3 /**< @hideinitializer \brief CZ_PLATFORM will have this value if we're building for iOS */
#define CZ_PLATFORM_ANDROID 4 /**< @hideinitializer \brief CZ_PLATFORM will have this value if we're building for Android */
#define CZ_PLATFORM_SYMBIAN 5 /**< @hideinitializer \brief CZ_PLATFORM will have this value if we're building for Symbian */
#define CZ_PLATFORM_MAC 6 /**< @hideinitializer \brief CZ_PLATFORM will have this value if we're building for Mac OSX */
#define CZ_ARCHITECTURE_32 1 /**< @hideinitializer \brief CZ_ARCH_TYPE will have this value if we're building for as 32 bits */
#define CZ_ARCHITECTURE_64 2 /**< @hideinitializer \brief CZ_ARCH_TYPE will have this value if we're building for as 64 bits */

/**
\def CZ_PLATFORM_SIMULATOR
@hideinitializer
\brief Defined as 0 or 1, depending if you're building for the platform simulator/emulator (if applicable).
*/

// Find platform
#if defined(SHP) || defined(__BADAOS__)
#	define CZ_PLATFORM CZ_PLATFORM_BADA
#	if defined( _WIN32 )
#		define CZ_PLATFORM_SIMULATOR 1
#	else
#		define CZ_PLATFORM_SIMULATOR 0
#	endif
#elif defined(__SYMBIAN__)
#   define CZ_PLATFORM CZ_PLATFORM_SYMBIAN
#   if defined(__WINS__)
#       define CZ_PLATFORM_SIMULATOR 1
#   else
#       define CZ_PLATFORM_SIMULATOR 0
#   endif
#elif defined(_WIN32)
#	define CZ_PLATFORM CZ_PLATFORM_WIN32
#	define CZ_PLATFORM_SIMULATOR 0
	#if (_WIN32_WINNT < 0x0501)
		#error Only supported for Windows XP or above. Set _WIN32_WINNT to 0x0501 or above
	#endif
#elif defined(__APPLE_CC__)
#	if TARGET_IPHONE_SIMULATOR)
#       define CZ_PLATFORM CZ_PLATFORM_IOS
#		define CZ_PLATFORM_SIMULATOR 1
#	elif TARGET_OS_IPHONE
#       define CZ_PLATFORM CZ_PLATFORM_IOS
#		define CZ_PLATFORM_SIMULATOR 0
#   elif TARGET_OS_MAC
#       define CZ_PLATFORM CZ_PLATFORM_MAC
#   else
#       error Unknown Aple OS/platform?
#	endif
#else
#	error Unsupported platform
#endif

/**
\def CZ_ARCH_TYPE
@hideinitializer
Defined to CZ_ARCHITECTURE_64 or CZ_ARCHITECTURE_32, depending on if you're building for 32 or 64 bits.
*/
// find architecture type
#if defined(__x86_64__) || defined(_M_X64) || defined(__powerpc64__) || defined(__alpha__) || defined(__ia64__) || defined(__s390__) || defined(__s390x__)
#   define CZ_ARCH_TYPE CZ_ARCHITECTURE_64
#else
#   define CZ_ARCH_TYPE CZ_ARCHITECTURE_32
#endif

/** \def CZ_DEBUG
@hideinitializer
\brief Defined as 0 or 1, depending if it's a debug build
*/
// Find release/debug/final build type
#if defined(DEBUG) || defined(_DEBUG) || defined(_DOXYGEN)
#	define CZ_DEBUG 1
#	if CZ_USER_BUILD
#		error "CZ_USER_BUILD can't be set for a debug build"
#	endif
#else
# if !defined(NDEBUG)
#	error No DEBUG/_DEBUG or NDEBUG defined
# endif
#	define CZ_DEBUG 0
#endif

/** \def CZ_USER_BUILD
@hideinitializer
\brief Defined as 0 or 1, depending if it's a final build
*/
// Detect if we're building a user build
#ifdef _DOXYGEN // If it's doxygen parsing this file, then assume it's a user build
#	define CZ_USER_BUILD 0
#	define CZ_PLATFORM CZ_PLATFORM_WIN32
#else
#	ifndef CZ_USER_BUILD // If CZ_USERBUILD was not defined in the project settings, then assume it's not a user build
#		define CZ_USER_BUILD 0
#	endif
#endif

/**
	@}
*/


// Including to use custom tweaks if any
#include "config/czPlatformConfigCustom.h"

//
// Detect and/or fix inconsistencies with the configuration, or assume
// defaults where no custom value specified

/** \name Build tweaks available
These defines can be changed to customize your build.
When not specified, default values will be used.
	@{
*/

#ifdef _DOXYGEN
	#define CZ_LOG_ENABLED 1 /**< @hideinitializer Set this to 0 or 1 to enable or disable logging */
#else
	#ifndef CZ_LOG_ENABLED
		#if CZ_USER_BUILD
			#define CZ_LOG_ENABLED 0
		#else
			#define CZ_LOG_ENABLED 1
		#endif
	#endif
#endif

#ifdef _DOXYGEN
	#define CZ_MEMORY_TRACKER_ENABLED 1 /**< @hideinitializer Set this to 0 or 1 to compile \link cz::MemoryTracker \endlink */
#else
	#ifndef CZ_MEMORY_TRACKER_ENABLED
		#if (CZ_PLATFORM == CZ_PLATFORM_WIN32) || (CZ_PLATFORM == CZ_PLATFORM_SYMBIAN)
			#if CZ_USER_BUILD
				#define CZ_MEMORY_TRACKER_ENABLED 0
			#else
				#define CZ_MEMORY_TRACKER_ENABLED 1
			#endif
		#else
			#define CZ_MEMORY_TRACKER_ENABLED 0
		#endif
	#endif
#endif

#ifdef _DOXYGEN
	#define CZ_CALLSTACKWALKER_ENABLED 1 /**< @hideinitializer Set this to 0 or 1 to compile \link cz::CallStack \endlink */
#else
	#ifndef CZ_CALLSTACKWALKER_ENABLED
		#if (CZ_PLATFORM == CZ_PLATFORM_WIN32) && !defined(__MINGW32__)
			#define CZ_CALLSTACKWALKER_ENABLED 1
		#else
			#define CZ_CALLSTACKWALKER_ENABLED 0
		#endif
	#endif
#endif


#ifdef _DOXYGEN
	#define CZ_THREADING_ENABLED 1 /**< @hideinitializer Set this to 0 or 1 to compile the threading classes */
#else
	#ifndef CZ_THREADING_ENABLED
		#if (CZ_PLATFORM == CZ_PLATFORM_WIN32) || (CZ_PLATFORM == CZ_PLATFORM_SYMBIAN)
			#define CZ_THREADING_ENABLED 1
		#else
			#define CZ_THREADING_ENABLED 0
		#endif
	#endif
#endif

//
// Enable/disable the use of some external libraries
#if (CZ_PLATFORM == CZ_PLATFORM_MAC) || (CZ_PLATFORM == CZ_PLATFORM_IOS) || (CZ_PLATFORM == CZ_PLATFORM_SYMBIAN) || (CZ_PLATFORM ==  CZ_PLATFORM_ANDROID)
    #define CZ_HAS_PTHREADS 1
#else
    #define CZ_HAS_PTHREADS 0
#endif

/**
	@}
*/

/**
	@}
*/

#endif

