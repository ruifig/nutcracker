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

// Detect processor architecture (http://predef.sourceforge.net/prearch.html)
#if defined(_M_IX86) || defined(_X86_) || defined(__INTEL__) || \
	defined(i386) || defined(__i386) || defined(__i386__) || \
	defined(__i486) || defined(__i486__) || \
	defined(__i586) || defined(__i586__) || \
	defined(__i686) || defined(__i686__)
#	define SHELL_CPUARCH "x86"
#elif defined(_M_X64) || defined(__amd64) || defined(__amd64__) || defined(__x86_64) || defined(__x86_64__)
#	define SHELL_CPUARCH "x64"
#	define SHELL_X64
#elif defined(_M_IA64) || defined(_IA64) || defined(__ia64) || defined(__ia64__) || defined(__IA64__)
#	define SHELL_CPUARCH "ia64"
#	define SHELL_X64
#elif defined(_M_ALPHA) || defined(__alpha) || defined(__alpha__) || defined(__ALPHA__)
#	define SHELL_CPUARCH "alpha"
#elif defined(_ARM) || defined(__arm__) || defined(__thumb__)
#	define SHELL_CPUARCH "arm"
#elif defined(mips) || defined(__mips) || defined(__mips__) || defined(__MIPS__)
#	define SHELL_CPUARCH "mips"
#elif defined(__hppa) || defined(__hppa__)
#	define SHELL_CPUARCH "hppa"
#elif defined(_M_PPC) || defined(_ARCH_PPC) || defined(__powerpc) || defined(__ppc__) || defined(__powerpc__) || defined(__POWERPC__)
#	define SHELL_CPUARCH "ppc"
#elif defined(__sparc) || defined(__sparc__)
#	define SHELL_CPUARCH "sparc"
#else
#	define SHELL_CPUARCH "unknown"
#endif

// Detect platform (http://predef.sourceforge.net/preos.html)
#if defined(WIN64) || defined(_WIN64) || defined(__WIN64__)
#	define SHELL_PLATFORM_WINDOWS
#	define SHELL_PLATFORM "win64"
#elif defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#	define SHELL_PLATFORM_WINDOWS
#	define SHELL_PLATFORM "win32"
#elif defined(__MINGW32__)
#	define SHELL_PLATFORM "mingw32"
#elif defined(MSDOS) || defined(_MSDOS) || defined(__MSDOS__)
#	define SHELL_PLATFORM "msdos"
#	define SHELL_PLATFORM_DOS
#elif defined(__CYGWIN32__)
#	define SHELL_PLATFORM "cygwin32"
#elif defined(linux) || defined(__linux) || defined(__linux__)
#	define SHELL_PLATFORM_LINUX
#	if defined(SHELL_X64)
#		define SHELL_PLATFORM "linux64"
#	else
#		define SHELL_PLATFORM "linux"
#	endif
#elif defined(OS2) || defined(_OS2) || defined(__OS2__) || defined(__EMX__)
#	define SHELL_PLATFORM "os2"
#elif defined(Macintosh) || defined(macintosh)
#	define SHELL_PLATFORM "macintosh"
#elif defined(__APPLE__) && defined(__GNUC__)
#	define SHELL_PLATFORM "macosx"
#elif defined(VMS) || defined(__VMS)
#	define SHELL_PLATFORM "vms"
#elif defined(__QNX__) || defined(__QNXNTO__)
#	define SHELL_PLATFORM "qnx"
#elif defined(NeXT) || defined(_NEXT_SOURCE)
#	define SHELL_PLATFORM "next"
#elif defined(__FreeBSD__)
#	define SHELL_PLATFORM "freebsd"
#elif defined(__NetBSD__)
#	define SHELL_PLATFORM "netbsd"
#elif defined(__OpenBSD__)
#	define SHELL_PLATFORM "openbsd"
#elif defined(__BSD__)
#	define SHELL_PLATFORM "bsd"
#elif defined(hpux) || defined(_hpux) || defined(__hpux)
#	define SHELL_PLATFORM "hpux"
#elif defined(sun) || defined(__sun)
#	if defined(__SVR4) || defined(__svr4__)
#		define SHELL_PLATFORM "solaris"
#	else
#		define SHELL_PLATFORM "sunos"
#	endif
#elif defined(__GNU__)
#	define SHELL_PLATFORM "hurd"
#elif defined(__SYMBIAN32__)
#	define SHELL_PLATFORM "symbian"
#else
#	define SHELL_PLATFORM "unknown"
#endif
