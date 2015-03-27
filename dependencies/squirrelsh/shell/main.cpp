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

#include "common.h"
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include "version.h"

// RVF +
#include <sqstdblob.h>
#include <sqstdsystem.h>
#include <sqstdio.h>
#include <sqstdmath.h>	
#include <sqstdstring.h>
#include <sqstdaux.h>
#include <sqrdbg.h>
// RVF -

#if defined(_MSC_VER)
#	include <conio.h>
#endif

#define MAX_CMD_LENGTH 32767 // Maximum length of one command in interactive mode

static bool stderrIsRedirected = false;

HSQUIRRELVM sqvm	= NULL;
bool		silent	= false;
int			retCode = 0;

extern void Init_Base		();
extern void Init_IO			();
extern void Init_File		();
extern void Init_Math		();
extern void Init_Util		();
extern void Init_Hash		();
extern void Init_RegExp		();

extern void Shutdown_Base	();
extern void Shutdown_IO		();
extern void Shutdown_File	();
extern void Shutdown_Math	();
extern void Shutdown_Util	();
extern void Shutdown_Hash	();
extern void Shutdown_RegExp ();

static void Shutdown (void)
{
	Shutdown_RegExp();
	Shutdown_Hash();
	Shutdown_Util();
	Shutdown_Math();
	Shutdown_File();
	Shutdown_IO();
	Shutdown_Base();

	_RPT0(_CRT_WARN, "--- Libraries deinitialized\n");

	if (sqvm)
		sq_close(sqvm);
	sqvm = NULL;

	_RPT0(_CRT_WARN, "--- Squirrel deinitialized\n");

#if defined(_MSC_VER) && defined(_DEBUG)
	// Wait for any key
	SetConsoleTitle("Squirrel Shell " SHELL_VERSION_STR " (" SHELL_CPUARCH ") - Finished");
	while (_kbhit())
		_getch();
	_getch();
#endif
}

// Output error message
void PrintError (const char* fmt, ...)
{
	// Don't output anything if silent mode is enabled
	if (silent)
		return;

	// Output error messages in red color. In Linux color is changed only if stderr isn't redirected. On other systems color
	// isn't changed
#if defined(SHELL_PLATFORM_WINDOWS)
	HANDLE					   stdErr = GetStdHandle(STD_ERROR_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(stdErr, &csbi);
	SetConsoleTextAttribute(stdErr, FOREGROUND_RED | FOREGROUND_INTENSITY);
#elif defined(SHELL_PLATFORM_LINUX)
	if (!stderrIsRedirected)
		fprintf(stderr, "\033[1;31m");
#endif

	va_list args;
	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);

	// Restore previous color
#if defined(SHELL_PLATFORM_WINDOWS)
	SetConsoleTextAttribute(stdErr, csbi.wAttributes);
#elif defined(SHELL_PLATFORM_LINUX)
	if (!stderrIsRedirected)
		fprintf(stderr, "\033[0m");
#endif
}

// Handle Squirrel compile errors
static void SquirrelCompileError (HSQUIRRELVM, const SQChar* desc, const SQChar*, SQInteger line, SQInteger column)
{
	PrintError("SQUIRREL ERROR: Line " FMT_INT ", Column " FMT_INT ": %s\n", line, column, desc);
}

// RVF +
#include <string>
std::string prepareFilename(const SQChar* filename);
// RVF -

// Load script from file
// Script is first read into memory buffer and then line beginning with "#!" (line with script interpreter specification in
// Linux) is converted into Squirrel comment. This line must be BEFORE actual script text
static bool LoadScript (const char* fileName)
{
	bool	res  = false;
	FILE*	file = NULL;
	SQChar* buf  = NULL;

	// RVF +
	std::string canonicalizedFilename = prepareFilename(fileName);
	fileName = canonicalizedFilename.c_str();
	// RVF -
	try
	{
#if defined(_MSC_VER) && (_MSC_VER >= 1400)
		if (fopen_s(&file, ConvPath(fileName, SQTrue), "rb"))
			file = NULL;
#else
		file = fopen(ConvPath(fileName, SQTrue), "rb");
#endif
		if (!file)
			throw "Failed to open the file";

		fseek(file, 0, SEEK_END);
		SQInteger size = SQInteger(ftell(file));
		if (!size)
			throw "File is empty";

		buf = (SQChar*)malloc(size + 1);
		if (!buf)
			throw "Not enough memory for script";

		fseek(file, 0, SEEK_SET);
		if (!fread(buf, size, 1, file))
			throw "Failed to read script";

		buf[size - 1] = 0;

		for (SQChar* c = buf; *c; ++c)
		{
			if (!isspace(*c))
			{
				if ((c[0] == '#') && (c[1] == '!'))
					c[0] = c[1] = '/';
				break;
			}
		}

		if (SQ_FAILED(sq_compilebuffer(sqvm, buf, size, fileName, SQTrue)))
			throw "Failed to compile script";

		res = true;
	}

	catch (const char* errMsg)
	{
		PrintError("ERROR: %s: %s.\n", fileName, errMsg);
	}

	if (file)
		fclose(file);

	free(buf);

	return res;
}

int main (int argc, char** argv)
{
	ENABLE_LEAK_CHECK();

#if defined(SHELL_PLATFORM_WINDOWS)
	SetConsoleTitle("Squirrel Shell " SHELL_VERSION_STR " (" SHELL_CPUARCH ")");
#else
	stderrIsRedirected = !isatty(2);
#endif

	// Parse command line arguments.
	const char* fileName	= NULL;
	bool		interactive = argc == 1;
	int			firstArg	= 0,
				i;
	bool isDebug = false;
	for (i = 1; argv[i]; ++i)
	{
		char* arg = argv[i];

		if (!strcmp(arg, "-h") || !strcmp(arg, "--help"))
		{
			printf("Squirrel Shell %s for %s on %s (" __DATE__ ")\n"
				   SHELL_VERSION_COPYRIGHT "\n"
				   "\n"
				   "This is free software, and comes WITHOUT ANY WARRANTY; without even the implied\n"
				   "warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU\n"
				   "General Public License for more details.\n"
				   "\n"
				   "MD5 hash calculation code (c) Colin Plumb\n"
				   "PCRE (c) University of Cambridge\n"
				   "Squirrel (c) Alberto Demichelis\n"
				   "zlib (c) Jean-loup Gailly and Mark Adler\n"
				   "\n"
				   "Usage:\n"
				   "   squirrelsh [options] [script_file] [script_arguments]\n"
				   "\n"
				   "Options:\n"
				   "   -h, --help          Display this text\n"
				   "   -d, --debug         Enable debugging information in script\n"
				   "   -i, --interactive   Run shell in interactive mode\n"
				   "                       If script file is specified, it will be executed before\n"
				   "                       entering this mode\n"
				   "   -s, --silent        Do not display error and warning messages\n"
				   "   -v, --version       Display shell version number\n"
				   "\n"
				   "Examples:\n"
				   "   squirrelsh                    Run shell in interactive mode\n"
				   "   squirrelsh foo.nut            Run foo.nut script without arguments\n"
				   "   squirrelsh -i foo.nut 1 2 3   Run foo.nut script with arguments \"1\", \"2\"\n"
				   "                                 and \"3\", and switch into interactive mode\n",
				   SHELL_VERSION_STR,
				   SHELL_PLATFORM,
				   SHELL_CPUARCH);

			return EXIT_SUCCESS;
		}
		else if (!strcmp(arg, "-d") || !strcmp(arg, "--debug"))
		{
			isDebug = true;
		}
		else if (!strcmp(arg, "-i") || !strcmp(arg, "--interactive"))
			interactive = true;
		else if (!strcmp(arg, "-v") || !strcmp(arg, "--version"))
		{
			printf("%s\n", SHELL_VERSION_STR);
			return EXIT_SUCCESS;
		}
		else if (!strcmp(arg, "-s") || !strcmp(arg, "--silent"))
			silent = true;
		else
		{
			// First unreserved argument will be treated as script file name.
			fileName = arg;
			firstArg = i;
			break;
		}
	}

	if (!fileName && !interactive)
	{
		PrintError("ERROR: Script file not specified.\n");
		return EXIT_FAILURE;
	}

	// Initialize Squirrel.
	sqvm = sq_open(1024);
	if (!sqvm)
	{
		PrintError("ERROR: Failed to create Squirrel VM.\n");
		return EXIT_FAILURE;
	}

	sqstd_seterrorhandlers(sqvm);

	HSQREMOTEDBG rdbg = nullptr;
	if (isDebug)
	{
		rdbg = sq_rdbg_init(sqvm, 1234, SQTrue);
		sq_enabledebuginfo(sqvm, SQTrue);

		//!! SUSPENDS THE APP UNTIL THE DEBUGGER CLIENT CONNECTS
		scprintf("Waiting for the debugger to connect...\n");
		if (!SQ_SUCCEEDED(sq_rdbg_waitforconnections(rdbg)))
		{
			PrintError("ERROR: Failed to connect to the debugger.\n");
			return EXIT_FAILURE;
		}
		scprintf(_SC("Connected to the debugger\n"));
	}


	//sq_setcompilererrorhandler(sqvm, SquirrelCompileError);

	_RPT0(_CRT_WARN, "--- Squirrel initialized\n");

	// Register some globals.
	SetSqString("SHELL_VERSION", SHELL_VERSION_STR, SQTrue);
	SetSqString("SQUIRREL_VERSION", SQUIRREL_VERSION_SHORT, SQTrue);
	SetSqString("PLATFORM", SHELL_PLATFORM, SQTrue);
	SetSqString("CPU_ARCH", SHELL_CPUARCH, SQTrue);

	// Initialize libraries.
	Init_Base();
	Init_IO();
	Init_File();
	Init_Math();
	Init_Util();
	Init_Hash();
	Init_RegExp();

	_RPT0(_CRT_WARN, "--- Libraries initialized\n");

	// Set up global variables...
	sq_pushroottable(sqvm);

	// RVF +
	// Initialize squirrel std libraries
	//sqstd_register_bloblib(sqvm);
	sqstd_register_iolib(sqvm); // We need this one because of the handy "dofile" function
	sqstd_register_stringfunctions(sqvm); // This registers only some string functions that are useful and don't clash with Squirrel Shell
	// NOTE: Not registering the other libraries, because there are name clashing between Squirrel Shell and SqStdLib
	//sqstd_register_systemlib(sqvm);
	//sqstd_register_mathlib(sqvm);
	//sqstd_register_stringlib(sqvm);
	// RVF -


	// ... number of command line arguments...
	sq_pushstring(sqvm, "__argc", -1);
	sq_pushinteger(sqvm, SQInteger(argc - firstArg));
	if (SQ_FAILED(sq_newslot(sqvm, -3, SQFalse)))
	{
		PrintError("ERROR: Failed to create \"__argc\" integer value.\n");
		Shutdown();
		return EXIT_FAILURE;
	}

	// ... and arguments themselves.
	sq_pushstring(sqvm, "__argv", -1);
	sq_newarray(sqvm, 0);
	for (i = firstArg; argv[i]; ++i)
	{
		sq_pushstring(sqvm, argv[i], -1);
		sq_arrayappend(sqvm, -2);
	}
	if (SQ_FAILED(sq_newslot(sqvm, -3, SQFalse)))
	{
		PrintError("ERROR: Failed to create \"__argv\" array.\n");
		Shutdown();
		return EXIT_FAILURE;
	}
	sq_pop(sqvm, 1);

	// Load and run script.
	SQInteger result = EXIT_SUCCESS;
	if (fileName && LoadScript(fileName))
	{
		sq_pushroottable(sqvm);
		if (SQ_FAILED(sq_call(sqvm, 1, SQTrue, isDebug ? SQTrue : SQFalse)))
		{
			if (!silent)
			{
				const SQChar* errMsg = "Unknown error.";
				sq_getlasterror(sqvm);
				if (sq_gettype(sqvm, -1) == OT_STRING)
					sq_getstring(sqvm, -1, &errMsg);

				PrintError("ERROR: %s\n", errMsg);
			}
			Shutdown();
			return EXIT_FAILURE;
		}

		// Get script execution result.
		if (sq_getvmstate(sqvm) == SQ_VMSTATE_SUSPENDED)
			result = retCode;
		else
		{
			if (sq_gettype(sqvm, -1) == OT_INTEGER)
				sq_getinteger(sqvm, -1, &result);
		}

		// Pop everything except root table.
		sq_settop(sqvm, 1);
	}

	// Enter interactive mode (if necessary).
	if (interactive)
	{
		SQChar cmd[MAX_CMD_LENGTH + 1];
		do
		{
#if defined(SHELL_PLATFORM_WINDOWS)
			GetCurrentDirectory(sizeof(cmd), cmd);
#else
			getcwd(cmd, sizeof(cmd));
#endif
			cmd[sizeof(cmd) - 1] = 0;

			printf("%s> ", ConvPath(cmd, SQFalse));
			fgets(cmd, MAX_CMD_LENGTH, stdin);
			if (SQ_FAILED(sq_compilebuffer(sqvm, cmd, SQInteger(strlen(cmd)), "", SQTrue)))
				continue;

			sq_pushroottable(sqvm);
			if (SQ_FAILED(sq_call(sqvm, 1, SQFalse, SQFalse)) && !silent)
			{
				const SQChar* errMsg = "Unknown error.";
				sq_getlasterror(sqvm);
				if (sq_gettype(sqvm, -1) == OT_STRING)
					sq_getstring(sqvm, -1, &errMsg);

				PrintError("ERROR: %s\n", errMsg);
			}
		} while(sq_getvmstate(sqvm) != SQ_VMSTATE_SUSPENDED);
		result = retCode;
	}

	if (isDebug)
	{
		sq_rdbg_shutdown(rdbg);
	}

	Shutdown();
	return int(result);
}
