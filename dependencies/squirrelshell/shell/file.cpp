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
#include <string.h>

#define MAX_OPENED_FILES 128 // Maximum number of simultaneously opened files

static FILE*	files[MAX_OPENED_FILES];
static unsigned numFiles = 0;

// Get file from Squirrel stack
static FILE* GetFile ()
{
	FILE* file;
	sq_getuserpointer(sqvm, 2, (SQUserPointer*)&file);
	if (file)
	{
		if (file == stdout || file == stderr)
			return file;

		for (unsigned i = 0; i < MAX_OPENED_FILES; ++i)
		{
			if (file == files[i])
				return file;
		}
	}
	return NULL;
}

// Close all opened files
static void CloseAllOpened ()
{
#if defined(_DEBUG)
	unsigned n = 0;
#endif

	for (unsigned i = 0; i < MAX_OPENED_FILES; ++i)
	{
		if (files[i])
		{
			fclose(files[i]);
#if defined(_DEBUG)
			++n;
#endif
		}
		files[i] = NULL;
	}
	numFiles = 0;

	_RPT1(_CRT_WARN, "--- CloseAllOpened() closed %u file(s)\n", n);
}

// handle fopen(string path, string mode)
// Open file
// 'mode' may be either one of modes used by fopen() function in libc or one of these values:
//    READ_ONLY  - open for reading only (the file must exist);
//    WRITE_ONLY - open for writing only;
//    READ_WRITE - open for both reading and writing (the file must exist);
//    APPEND     - open for appending (writing data at the end of the file).
static SQInteger FOpen (HSQUIRRELVM)
{
	if (numFiles == MAX_OPENED_FILES)
	{
		PrintError("ERROR: Too many opened files.\n");
		sq_pushnull(sqvm);
		return 1;
	}

	// Find first unopened file
	unsigned i;
	for (i = 0; i < MAX_OPENED_FILES; ++i)
	{
		if (!files[i])
			break;
	}

	const SQChar *path, *mode;
	sq_getstring(sqvm, 2, &path);
	sq_getstring(sqvm, 3, &mode);

	// Binary file I/O isn't provided to scripts
	// FIXME: Wouldn't binary file I/O be useful?
	SQChar* c = (SQChar*)strchr(mode, 'b');
	if (c)
		*c = 0;

#if defined(_MSC_VER) && (_MSC_VER >= 1400)
	if (fopen_s(&files[i], ConvPath(path, SQTrue), mode))
		files[i] = NULL;
#else
	files[i] = fopen(ConvPath(path, SQTrue), mode);
#endif

	FILE* res = files[i];
	if (res)
	{
		sq_pushuserpointer(sqvm, res);
		++numFiles;
		_RPT2(_CRT_WARN, "--- Opened file #%d. %d file(s) are opened.\n", i, numFiles);
	}
	else
		sq_pushnull(sqvm);
	return 1;
}

// fprint(handle file, string text)
// Output text to file
static SQInteger FPrint (HSQUIRRELVM)
{
	FILE* file = GetFile();
	if (file)
	{
		const SQChar* text;
		sq_getstring(sqvm, 3, &text);
		fputs(text, file);
	}
	return 0;
}

// fprintl(handle file[, string text])
// Output text with trailing linefeed to file
static SQInteger FPrintL (HSQUIRRELVM)
{
	FILE* file = GetFile();
	if (file)
	{
		if (sq_gettop(sqvm) == 3)
		{
			const SQChar* text;
			sq_getstring(sqvm, 3, &text);
			fprintf(file, "%s\n", text);
		}
		else
			fprintf(file, "\n");
	}
	return 0;
}

// variant fscan(handle file[, integer type = TEXT])
// Read data from file
// For list of supported types look 'scan' function specification
// If invalid file handle or unsupported type specified, read nothing and return 'null' value
static SQInteger FScan (HSQUIRRELVM)
{
	FILE* file = GetFile();
	if (file)
	{
		SQInteger type = 't';
		if (sq_gettop(sqvm) == 3)
			sq_getinteger(sqvm, 3, &type);

		SQInteger intVal;
		switch (type)
		{
		case 't':
			SQChar buf[4096];
			fgets(buf, sizeof(buf), file);
			buf[sizeof(buf) - 1] = 0;
			for (SQChar* c = buf; *c; ++c)
			{
				if (*c == '\n')
				{
					*c = 0;
					break;
				}
			}
			sq_pushstring(sqvm, buf, -1);
			break;

		case 'c':
			sq_pushinteger(sqvm, fgetc(file));
			break;

		case 'i':
			fscanf_s(file, FMT_INT, &intVal);
			sq_pushinteger(sqvm, intVal);
			break;

		case 'u':
			fscanf_s(file, FMT_UINT, &intVal);
			sq_pushinteger(sqvm, intVal);
			break;

		case 'f':
			SQFloat fltVal;
			fscanf_s(file, "%f", &fltVal);
			sq_pushfloat(sqvm, fltVal);
			break;

		default:
			sq_pushnull(sqvm);
		}
	}
	else
		sq_pushnull(sqvm);
	return 1;
}

// fclose(handle file)
// Close file
static SQInteger FClose (HSQUIRRELVM)
{
	FILE* file;
	sq_getuserpointer(sqvm, 2, (SQUserPointer*)&file);
	if (file)
	{
		for (unsigned i = 0; i < MAX_OPENED_FILES; ++i)
		{
			if (file != files[i])
				continue;

			fclose(file);
			files[i] = NULL;
			--numFiles;
			_RPT2(_CRT_WARN, "--- Closed file #%d. %d file(s) are still opened.\n", i, numFiles);
			break;
		}
	}
	return 0;
}

// fcloseall()
// Close all opened files
static SQInteger FCloseAll (HSQUIRRELVM)
{
	CloseAllOpened();
	return 0;
}

void Init_File ()
{
	for (unsigned i = 0; i < MAX_OPENED_FILES; ++i)
		files[i] = NULL;

	SetSqString("READ_ONLY", "r", SQTrue);
	SetSqString("WRITE_ONLY", "w", SQTrue);
	SetSqString("READ_WRITE", "r+", SQTrue);
	SetSqString("APPEND", "a", SQTrue);
	SetSqInteger("TEXT", 't', SQTrue);
	SetSqInteger("CHAR", 'c', SQTrue);
	SetSqInteger("INT", 'i', SQTrue);
	SetSqInteger("UINT", 'u', SQTrue);
	SetSqInteger("FLOAT", 'f', SQTrue);
	SetSqFunction("fopen", FOpen, 3, ". s s");
	SetSqFunction("fprint", FPrint, 3, ". p s");
	SetSqFunction("fprintl", FPrintL, -2, ". p s");
	SetSqFunction("fscan", FScan, -2, ". p i");
	SetSqFunction("fclose", FClose, 2, ". p");
	SetSqFunction("fcloseall", FCloseAll, 1, ".");

	_RPT0(_CRT_WARN, "------ File I/O library initialized\n");
}

void Shutdown_File ()
{
	CloseAllOpened();
	_RPT0(_CRT_WARN, "------ File I/O library deinitialized\n");
}
