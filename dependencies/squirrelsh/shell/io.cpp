// Squirrel Shell
// Copyright (c) 2006-2008, Constantin Makshin
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

#include "common.h"
#include <stdarg.h>

// clear()
// Clear console
static SQInteger Clear (HSQUIRRELVM)
{
#if defined(SHELL_PLATFORM_WINDOWS) || defined(SHELL_PLATFORM_DOS)
	system("cls");
#else
	system("clear");
#endif
	return 0;
}

// print(string text)
// Print text to the standard output stream
static void Print (HSQUIRRELVM, const SQChar* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vfprintf(stdout, fmt, args);
	va_end(args);
}

// Print text to the standard error stream
static void PrintError (HSQUIRRELVM, const SQChar* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);
}

// printl([string text])
// Output text to console and end it with linefeed
static SQInteger PrintL (HSQUIRRELVM)
{
	const SQChar* str;
	sq_getstring(sqvm, 2, &str);
	printf("%s\n", str);
	return 0;
}

// variant scan([integer type = 't'])
// Input data from console.
// Supported types:
//    TEXT  ('t') - text (default);
//    CHAR  ('c') - single character;
//    INT   ('i') - signed integer;
//    UINT  ('u') - unsigned integer;
//    FLOAT ('f') - float.
// If unsupported type specified, return 'null' value
static SQInteger Scan (HSQUIRRELVM)
{
	char buf[4096];
	fgets(buf, sizeof(buf), stdin);
	buf[sizeof(buf) - 1] = 0;
	for (char* c = buf; *c; ++c)
	{
		if (*c == '\n')
		{
			*c = 0;
			break;
		}
	}

	SQInteger type = 't';
	if (sq_gettop(sqvm) == 2)
		sq_getinteger(sqvm, 2, &type);
	switch (type)
	{
	case 't':
		sq_pushstring(sqvm, buf, -1);
		break;

	case 'c':
		sq_pushinteger(sqvm, buf[0]);
		break;

	case 'i':
		sq_pushinteger(sqvm, SQInteger(strtol(buf, NULL, 10)));
		break;

	case 'u':
		sq_pushinteger(sqvm, SQInteger(strtoul(buf, NULL, 10)));
		break;

	case 'f':
		sq_pushfloat(sqvm, SQFloat(strtod(buf, NULL)));
		break;

	default:
		sq_pushnull(sqvm);
	}
	return 1;
}

void Init_IO ()
{
	SetSqInteger("TEXT", 't', SQTrue);
	SetSqInteger("CHAR", 'c', SQTrue);
	SetSqInteger("INT", 'i', SQTrue);
	SetSqInteger("UINT", 'u', SQTrue);
	SetSqInteger("FLOAT", 'f', SQTrue);
	SetSqFunction("clear", Clear, 1, ".");
	sq_setprintfunc(sqvm, Print, PrintError);
	SetSqFunction("printl", PrintL, -1, ". s");
	SetSqFunction("scan", Scan, -1, ". i");

	_RPT0(_CRT_WARN, "------ I/O library initialized\n");
}

void Shutdown_IO ()
{
	_RPT0(_CRT_WARN, "------ I/O library deinitialized\n");
}
