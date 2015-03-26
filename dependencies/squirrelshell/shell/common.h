// Squirrel Shell
// Copyright (c) 2006-2010, Constantin Makshin
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#pragma once

#if (defined(_UNICODE) || defined(UNICODE))
#	error "Currently Unicode isn't supported"
#endif

#if (!defined(_DEBUG) && !defined(NDEBUG))
#	define _DEBUG
#endif

#include "platform.h"

// Define *printf() format for SQ[Unsigned]Integer
#if defined(SHELL_X64)
#	if !defined(_SQ64)
#		define _SQ64
#	endif
#	if defined(_MSC_VER)
#		define FMT_INT  "%I64d"
#		define FMT_UINT "%I64u"
#	else
#		define FMT_INT  "%ld"
#		define FMT_UINT "%lu"
#	endif
#else
#	define FMT_INT  "%d"
#	define FMT_UINT "%u"
#endif

#if defined(SHELL_PLATFORM_WINDOWS)
#	define  WIN32_LEAN_AND_MEAN
#	define  WIN64_LEAN_AND_MEAN
#	define  STRICT
#	include <windows.h>
#else
#	include <unistd.h>
#endif

#if !defined(_MSC_VER) || (_MSC_VER < 1400)
// Use safe functions in Visual Studio .NET 2005 and standard with other compilers
#	define strcat_s(dest, size, src)		 strcat(dest, src)
#	define strncpy_s(dest, size, src, count) strncpy(dest, src, count)
#	define fscanf_s							 fscanf
#endif

#include <stdio.h>
#include <stdlib.h>
#include "squirrel.h"

// Define debugging macros
#if defined(_MSC_VER)
#	include <crtdbg.h>
#	define  ENABLE_LEAK_CHECK() _CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF)
#else
#	define ENABLE_LEAK_CHECK()
#	if defined(_DEBUG)
#		define _RPT0(rptno, msg)						 fprintf(stderr, msg)
#		define _RPT1(rptno, msg, arg1)					 fprintf(stderr, msg, arg1)
#		define _RPT2(rptno, msg, arg1, arg2)			 fprintf(stderr, msg, arg1, arg2)
#		define _RPT3(rptno, msg, arg1, arg2, arg3)		 fprintf(stderr, msg, arg1, arg2, arg3)
#		define _RPT4(rptno, msg, arg1, arg2, arg3, arg4) fprintf(stderr, msg, arg1, arg2, arg3, arg4)
#	else
#		define _RPT0(rptno, msg)
#		define _RPT1(rptno, msg, arg1)
#		define _RPT2(rptno, msg, arg1, arg2)
#		define _RPT3(rptno, msg, arg1, arg2, arg3)
#		define _RPT4(rptno, msg, arg1, arg2, arg3, arg4)
#	endif
#endif

#if !defined(MAX_PATH)
#	define MAX_PATH 260
#endif

#if !defined(min)
#	define min(a, b) ((a) < (b) ? (a) : (b))
#endif

#if !defined(max)
#	define max(a, b) ((a) > (b) ? (a) : (b))
#endif

#define SQUIRREL_VERSION_SHORT "3.0.3"

extern HSQUIRRELVM sqvm;	// We aren't going to create more than one VM, so it's acceptable to make this global
extern bool		   silent;
extern int		   retCode; // Ugly hack to allow exit() shell function gracefully close the shell

// Set value of a Squirrel integer variable/constant
void		  SetSqInteger	(const SQChar* name, SQInteger value, SQBool isConst = SQFalse);

// Set value of a Squirrel floating point variable/constant
void		  SetSqFloat	(const SQChar* name, SQFloat value, SQBool isConst = SQFalse);

// Set value of a Squirrel string variable/constant
void		  SetSqString	(const SQChar* name, const SQChar* value, SQBool isConst = SQFalse);

// Set native function callable from Squirrel scripts
void		  SetSqFunction (const SQChar* name, SQFUNCTION func, SQInteger numParams, const SQChar* params);

// Convert path to/from OS's native format
const SQChar* ConvPath		(const SQChar* path, SQBool toNative);

// Output error message
void		  PrintError	(const char* fmt, ...);
