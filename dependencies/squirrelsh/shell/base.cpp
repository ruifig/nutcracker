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
#include <string.h>
#include <string>

#if defined(SHELL_PLATFORM_WINDOWS)
typedef HANDLE SysHandle;
#else
#	include <sys/stat.h>
#	include <sys/types.h>
#	include <sys/wait.h>
#	include <dirent.h>
#	include <fcntl.h>
#	include <grp.h>
#	include <pwd.h>
#	include <signal.h>
#	include <time.h>
#	include <utime.h>
#	define  INVALID_HANDLE_VALUE (-1)

typedef	int SysHandle;
#endif

#if !defined(min)
#	define min(a, b) ((a) < (b) ? (a) : (b))
#endif

#if !defined(max)
#	define max(a, b) ((a) > (b) ? (a) : (b))
#endif

// Maximum number of command line arguments passed to the child process
#define MAX_ARGS		 130
// Maximum number of environment variables passed to the child process
#define MAX_ENVS		 129

// Size of the temporary buffer used to get data from child process' output and error streams
#define PIPE_BUFFER_SIZE (512 * 1024) // 512 KiB

#define FILETIME_CREATE  0
#define FILETIME_ACCESS  1
#define FILETIME_MODIFY  2
#define FILETIME_CHANGE  3

extern SQInteger TimeToInt (unsigned, unsigned, unsigned, unsigned, unsigned, unsigned);
extern void		 IntToTime (SQInteger, unsigned&, unsigned&, unsigned&, unsigned&, unsigned&, unsigned&);

#if !defined(SHELL_PLATFORM_WINDOWS)
// Copy data from one file to another.
static void CopyFile (const SQChar* from, const SQChar* to, SQBool doNotOverwrite)
{
	if (doNotOverwrite && !access(to, F_OK))
		return;

	FILE *src  = fopen(from, "rb"),
		 *dest = fopen(to, "wb");
	if (src && dest)
	{
		unsigned char buf[4096];
		for (;;)
		{
			size_t n = fread(buf, 1, sizeof(buf), src);
			if (!n)
				break;
			fwrite(buf, 1, n, dest);
		}
	}
	if (dest)
		fclose(dest);
	if (src)
		fclose(src);
}
#endif

// Remove directory with all its contents
static void RmDirRecursively (const SQChar* path)
{
	SQChar tmp[MAX_PATH];

#if defined(SHELL_PLATFORM_WINDOWS)
	// Append wildcard to the end of path
#if (_MSC_VER >= 1400)
	_snprintf_s(tmp, MAX_PATH, MAX_PATH - 1, "%s\\*", path);
#else
	_snprintf(tmp, MAX_PATH - 1, "%s\\*", path);
#endif

	// List files
	WIN32_FIND_DATA fd;
	HANDLE			fh = FindFirstFile(tmp, &fd);
	if (fh != INVALID_HANDLE_VALUE)
	{
		// First 2 entries will be "." and ".." - skip them
		FindNextFile(fh, &fd);
		while (FindNextFile(fh, &fd))
		{
#if (_MSC_VER >= 1400)
			_snprintf_s(tmp, MAX_PATH, MAX_PATH - 1, "%s/%s", path, fd.cFileName);
#else
			_snprintf(tmp, MAX_PATH - 1, "%s/%s", path, fd.cFileName);
#endif

			DWORD attrs = GetFileAttributes(tmp);
			if (attrs == INVALID_FILE_ATTRIBUTES)
				continue;

			if (attrs & FILE_ATTRIBUTE_DIRECTORY)
				RmDirRecursively(tmp);
			else
				DeleteFile(tmp);
		}
		FindClose(fh);
	}
	RemoveDirectory(path);
#else // SHELL_PLATFORM_WINDOWS
	DIR* dir = opendir(path);
	if (dir)
	{
		// As in Windows, first 2 entries are "." and ".."
		readdir(dir);
		readdir(dir);

		dirent* de;
		while ((de = readdir(dir)) != NULL)
		{
			snprintf(tmp, MAX_PATH - 1, "%s/%s", path, de->d_name);
			struct stat fileStat;
			if (stat(tmp, &fileStat))
				continue;

			if (S_ISDIR(fileStat.st_mode))
				RmDirRecursively(tmp);
			else
				remove(tmp);
		}
		closedir(dir);
	}
	rmdir(path);
#endif // SHELL_PLATFORM_WINDOWS
}

// Close system handle
static inline void CloseSysHandle (SysHandle& handle)
{
	if (handle == INVALID_HANDLE_VALUE)
		return;

#if defined(SHELL_PLATFORM_WINDOWS)
	CloseHandle(handle);
#else
	close(handle);
#endif
	handle = INVALID_HANDLE_VALUE;
}

// Read data from a pipe and return 'false' if nothing has been read
static bool ReadFromPipe (SysHandle pipe, void* buffer, size_t numBytesToRead, size_t* numBytesRead)
{
#if defined(SHELL_PLATFORM_WINDOWS)
	DWORD numBytesAvailable,
		  nbr;

	if (!numBytesToRead ||
		!PeekNamedPipe(pipe, NULL, 0, NULL, &numBytesAvailable, NULL) || !numBytesAvailable ||
		!ReadFile(pipe, buffer, min(numBytesToRead, numBytesAvailable), &nbr, NULL) || !nbr)
	{
		return false;
	}
	if (numBytesRead)
		*numBytesRead = nbr;

	return true;
#else
	int nbr = read(pipe, buffer, numBytesToRead);
	if (numBytesRead)
		*numBytesRead = max(nbr, 0);

	return nbr > 0;
#endif
}

// Write data to a pipe and return 'false' if nothing has been written
static bool WriteToPipe (SysHandle pipe, const void* buffer, size_t numBytesToWrite, size_t* numBytesWritten)
{
#if defined(SHELL_PLATFORM_WINDOWS)
	DWORD nbw;
	if (!numBytesToWrite || !WriteFile(pipe, buffer, numBytesToWrite, &nbw, NULL) || !nbw)
	{
		return false;
	}
	if (numBytesWritten)
		*numBytesWritten = nbw;

	return true;
#else
	int nbw = write(pipe, buffer, numBytesToWrite);

	if (numBytesWritten)
		*numBytesWritten = max(nbw, 0);

	return nbw > 0;
#endif
}

// bool mkdir(string path[, integer mode = 0755])
// Create new directory
static SQInteger MkDir (HSQUIRRELVM)
{
	const SQChar* path;
	sq_getstring(sqvm, 2, &path);

#if defined(SHELL_PLATFORM_WINDOWS)
	sq_pushbool(sqvm, CreateDirectory(ConvPath(path, SQTrue), NULL));
#else
	SQInteger mode = 0755;
	if (sq_gettop(sqvm) == 3)
		sq_getinteger(sqvm, 3, &mode);

	sq_pushbool(sqvm, !mkdir(ConvPath(path, SQTrue), mode_t(mode)));
#endif
	return 1;
}

// string getcwd()
// Get current directory
static SQInteger GetCWD (HSQUIRRELVM)
{
	SQChar path[MAX_PATH];

#if defined(SHELL_PLATFORM_WINDOWS)
	GetCurrentDirectory(sizeof(path), path);
#else
	getcwd(path, sizeof(path));
#endif
	path[sizeof(path) - 1] = 0;

	sq_pushstring(sqvm, ConvPath(path, SQTrue), -1);
	return 1;
}

// bool chdir(string path)
// Set current directory
static SQInteger ChDir (HSQUIRRELVM)
{
	const SQChar* path;
	sq_getstring(sqvm, 2, &path);

#if defined(SHELL_PLATFORM_WINDOWS)
	sq_pushbool(sqvm, SetCurrentDirectory(ConvPath(path, SQTrue)));
#else
	sq_pushbool(sqvm, !chdir(ConvPath(path, SQTrue)));
#endif

	return 1;
}

// bool exist(string path)
// Check whether file or directory exists or not
static SQInteger Exist (HSQUIRRELVM)
{
	const SQChar* path;
	sq_getstring(sqvm, 2, &path);

#if defined(SHELL_PLATFORM_WINDOWS)
	sq_pushbool(sqvm, (GetFileAttributes(ConvPath(path, SQTrue)) != INVALID_FILE_ATTRIBUTES));
#else
	sq_pushbool(sqvm, !access(ConvPath(path, SQTrue), F_OK));
#endif

	return 1;
}

// copy(string from, string to[, bool overwrite = false])
// Copy file
static SQInteger Copy (HSQUIRRELVM)
{
	const SQChar *from,
				 *to;
	SQBool		 ovr = SQFalse;
	sq_getstring(sqvm, 2, &from);
	sq_getstring(sqvm, 3, &to);
	if (sq_gettop(sqvm) == 4)
		sq_getbool(sqvm, 4, &ovr);

	// Copy paths in OS's native format to temporary buffers (in functions with several path arguments ConvPath()'s
	// static buffer may cause problems)
	SQChar src[MAX_PATH], dest[MAX_PATH];
	strncpy_s(src, sizeof(src), ConvPath(from, SQTrue), MAX_PATH);
	strncpy_s(dest, sizeof(dest), ConvPath(to, SQTrue), MAX_PATH);
	BOOL res = CopyFile(src, dest, !ovr);

	// RVF +
	sq_pushbool(sqvm, (res) ? SQTrue : SQFalse);
	// RVF -

#if !defined(SHELL_PLATFORM_WINDOWS)
	// Copy timestamps and permissions
	struct stat fileStat;
	if (!stat(src, &fileStat))
	{
		utimbuf fileTime = { fileStat.st_atime, fileStat.st_mtime };
		chmod(dest, fileStat.st_mode);
		utime(dest, &fileTime);
	}
#endif
	return 1;
}

// move(string from, string to)
// Move file
static SQInteger Move (HSQUIRRELVM)
{
	const SQChar *from,
				 *to;
	sq_getstring(sqvm, 2, &from);
	sq_getstring(sqvm, 3, &to);
	SQChar src[MAX_PATH], dest[MAX_PATH];
	strncpy_s(src, sizeof(src), ConvPath(from, SQTrue), MAX_PATH);
	strncpy_s(dest, sizeof(dest), ConvPath(to, SQTrue), MAX_PATH);

#if defined(SHELL_PLATFORM_WINDOWS)
	MoveFile(src, dest);
#else
	rename(src, dest);
#endif
	return 0;
}

// remove(string path)
// Delete file
static SQInteger Remove (HSQUIRRELVM)
{
	const SQChar* path;
	sq_getstring(sqvm, 2, &path);

#if defined(SHELL_PLATFORM_WINDOWS)
	DeleteFile(ConvPath(path, SQTrue));
#else
	remove(ConvPath(path, SQTrue));
#endif
	return 0;
}

// rmdir(string path[, bool recursive = false])
// Remove directory (must be empty unless 'recursive' is 'true')
static SQInteger RmDir (HSQUIRRELVM)
{
	const SQChar* path;
	sq_getstring(sqvm, 2, &path);

	SQChar tmp[MAX_PATH];
	strncpy_s(tmp, sizeof(tmp), ConvPath(path, SQFalse), MAX_PATH);

	// Remove trailing slashes
	for (SQChar* c = &tmp[sq_getsize(sqvm, 2) - 1]; (c >= tmp) && (*c == '/'); --c)
		*c = 0;

	SQBool recursive = SQFalse;
	if (sq_gettop(sqvm) == 3)
		sq_getbool(sqvm, 3, &recursive);

	if (recursive)
		RmDirRecursively(ConvPath(tmp, SQTrue));
	else
	{
#if defined(SHELL_PLATFORM_WINDOWS)
		RemoveDirectory(tmp);
#else
		rmdir(tmp);
#endif
	}
	return 0;
}

// string getenv(string name)
// Get value of environment variable
static SQInteger GetEnv (HSQUIRRELVM)
{
	if (!sq_getsize(sqvm, 2))
	{
		sq_pushnull(sqvm);
		return 1;
	}

	const SQChar* name;
	sq_getstring(sqvm, 2, &name);

#if defined(SHELL_PLATFORM_WINDOWS)
	SQChar res[32767];
	DWORD  size = sizeof(res) / sizeof(SQChar);
	if (GetEnvironmentVariable(name, res, size))
#else
	SQChar* res = getenv(name);
	if (res)
#endif
		sq_pushstring(sqvm, res, -1);
	else
		sq_pushnull(sqvm);
	return 1;
}

// bool setenv(string name, string value)
// Set value of environment variable
static SQInteger SetEnv (HSQUIRRELVM)
{
	if (!sq_getsize(sqvm, 2))
	{
		sq_pushbool(sqvm, SQFalse);
		return 1;
	}

	const SQChar *name, *value;
	sq_getstring(sqvm, 2, &name);
	sq_getstring(sqvm, 3, &value);

#if defined(SHELL_PLATFORM_WINDOWS)
	sq_pushbool(sqvm, SetEnvironmentVariable(name, value));
#else
	sq_pushbool(sqvm, !setenv(name, value, true));
#endif
	return 1;
}

// delenv(string name)
// Remove environment variable
static SQInteger DelEnv (HSQUIRRELVM)
{
	if (!sq_getsize(sqvm, 2))
		return 0;

	const SQChar* name;
	sq_getstring(sqvm, 2, &name);

#if defined(SHELL_PLATFORM_WINDOWS)
	SetEnvironmentVariable(name, NULL);
#else
	unsetenv(name);
#endif
	return 0;
}

// integer run(string path[, strarray args[, string input = null[, bool redirectStdout[, bool redirectStderr]]]])
// Run another program or script
// 'path' is passed as first argument automatically, so there's no need to set is manually
// Return child exit code or -1 if failed
static SQInteger Run (HSQUIRRELVM)
{
	if (!sq_getsize(sqvm, 2))
	{
		sq_pushinteger(sqvm, -1);
		return 1;
	}

	const SQChar* tmp;
	sq_getstring(sqvm, 2, &tmp);

	std::basic_string<SQChar> path = ConvPath(tmp, SQTrue);
#if defined(SHELL_PLATFORM_WINDOWS)
	// Add "exe" file extension if necessary
	size_t bs  = path.rfind('\\'),
		   ext = path.rfind('.');
	if ((ext == std::basic_string<SQChar>::npos) || (ext < bs))
		path += ".exe";

	// Search for the file in directories listed in the PATH environment variable and some other places
	// Don't search if path is specified
	if (bs == std::basic_string<SQChar>::npos)
	{
		SQChar fullPath[MAX_PATH + 1];
		if (!SearchPath(NULL, path.c_str(), NULL, sizeof(fullPath) - 1, fullPath, NULL))
		{
			sq_pushinteger(sqvm, -1);
			return 1;
		}

		fullPath[sizeof(fullPath) - 1] = 0;
		path = fullPath;
	}
#endif

	SQChar *args[MAX_ARGS] = { (SQChar*)path.c_str(), NULL };
	size_t i,
		   numArgs			 = 1,
		   commandLineLength = 0;

	// Build list of command line arguments
	if ((sq_gettop(sqvm) >= 3) && (sq_getsize(sqvm, 3) > 0))
	{
		if (sq_gettype(sqvm, 3) == OT_STRING)
		{
			const SQChar** arg = (const SQChar**)&args[numArgs];
			if (SQ_SUCCEEDED(sq_getstring(sqvm, 3, arg)))
			{
				++numArgs;

				// The parameter'll be enclosed in quotes and prefixed with space
				commandLineLength += strlen(*arg) + 3;
			}
		}
		else
		{
			SQInteger arraySize = sq_getsize(sqvm, 3);
			if (arraySize < 0)
				arraySize = 0; // Make sure it's safe to cast 'arraySize' to an unsigned type
			else if (arraySize > (MAX_ARGS - 2))
				arraySize = MAX_ARGS - 2;

			sq_pushnull(sqvm);
			for (i = 0; i < size_t(arraySize) && SQ_SUCCEEDED(sq_next(sqvm, 3)); ++i)
			{
				if (sq_gettype(sqvm, -1) != OT_STRING)
				{
					sq_pop(sqvm, 3); // Pop the key, value and iterator
					return sq_throwerror(sqvm, "'args' parameter of the run() function must must be an array of strings");
				}

				if (SQ_SUCCEEDED(sq_getstring(sqvm, -1, &tmp)))
				{
					size_t argSize = strlen(tmp) + 1;
					args[numArgs] = (SQChar*)malloc(argSize);
					if (!args[numArgs])
					{
						PrintError("ERROR: Not enough memory.\n");
						break;
					}

					strncpy_s(args[numArgs], argSize, tmp, argSize);
					++numArgs;

					commandLineLength += argSize + 2; // Each argument'll be enclosed in quotes
				}
				sq_pop(sqvm, 2); // Pop the key and value
			}
			sq_pop(sqvm, 1); // Pop the iterator
		}
		args[numArgs] = NULL;
	}

	// Determine what streams should be redirected
	SQBool		  redirectStdin	 = SQFalse,
				  redirectStdout = SQFalse,
				  redirectStderr = SQFalse;
	const SQChar* input			 = NULL;
	SQInteger	  inputLength	 = sq_gettop(sqvm) >= 4 ? sq_getsize(sqvm, 4) : 0;

	if (inputLength > 0)
		redirectStdin = SQ_SUCCEEDED(sq_getstring(sqvm, 4, &input));
	if (sq_gettop(sqvm) >= 5)
		sq_getbool(sqvm, 5, &redirectStdout);
	if (sq_gettop(sqvm) == 6)
		sq_getbool(sqvm, 6, &redirectStderr);

	SysHandle newInput[2]  = { INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE },
			  newOutput[2] = { INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE },
			  newError[2]  = { INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE };
	SQInteger res		   = -1;

#if defined(SHELL_PLATFORM_WINDOWS)
	// Here we need to pack all parameters into one string...
	commandLineLength += SQInteger(path.length() + 3); // Add space for the quoted path to the executable and termination '\0'

	std::string commandLine;
	commandLine.reserve(commandLineLength);

	commandLine =  '\"';
	commandLine += path;
	commandLine += '\"';

	for (i = 1; i < numArgs; ++i)
	{
		commandLine += " \"";
		commandLine += args[i];
		commandLine += "\"";
	}

	// Create pipes for requested redirections
	SECURITY_ATTRIBUTES sa;
	memset(&sa, 0, sizeof(SECURITY_ATTRIBUTES));
	sa.nLength		  = sizeof(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = TRUE;

	if (redirectStdin)
	{
		if (!CreatePipe(&newInput[0], &newInput[1], &sa, PIPE_BUFFER_SIZE))
		{
			PrintError("ERROR: Failed to create input pipe.\n");
			goto finish;
		}
		SetHandleInformation(newInput[1], HANDLE_FLAG_INHERIT, 0);
	}
	else
		newInput[0] = GetStdHandle(STD_INPUT_HANDLE);

	if (redirectStdout)
	{
		if (!CreatePipe(&newOutput[0], &newOutput[1], &sa, PIPE_BUFFER_SIZE))
		{
			PrintError("ERROR: Failed to create output pipe.\n");
			goto finish;
		}
		SetHandleInformation(newOutput[0], HANDLE_FLAG_INHERIT, 0);
	}
	else
		newOutput[1] = GetStdHandle(STD_OUTPUT_HANDLE);

	if (redirectStderr)
	{
		if (!CreatePipe(&newError[0], &newError[1], &sa, PIPE_BUFFER_SIZE))
		{
			PrintError("ERROR: Failed to create error pipe.\n");
			goto finish;
		}
		SetHandleInformation(newError[0], HANDLE_FLAG_INHERIT, 0);
	}
	else
		newError[1] = GetStdHandle(STD_ERROR_HANDLE);

	// Run requested process
	_RPT1(_CRT_WARN, "--- Running '%s'...\n", commandLine.c_str());
	STARTUPINFO			si;
	PROCESS_INFORMATION pi;
	memset(&si, 0, sizeof(STARTUPINFO));
	si.cb		  = sizeof(STARTUPINFO);
	si.dwFlags	  = STARTF_USESTDHANDLES;
	si.hStdInput  = newInput[0];
	si.hStdOutput = newOutput[1];
	si.hStdError  = newError[1];
	if (CreateProcess(path.c_str(), (LPSTR)commandLine.c_str(), NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi))
	{
		CloseHandle(pi.hThread);

		if (!redirectStdin && !redirectStdout && !redirectStderr)
			WaitForSingleObject(pi.hProcess, INFINITE);
		else
		{
			// Give the child process a chance to produce some output
			Sleep(50);

			// Pass data to/from child process' streams
			std::basic_string<SQChar> output,
									  error;
			SQChar					  buffer[PIPE_BUFFER_SIZE];
			size_t					  numBytesRead;
			do
			{
				// ... stdin
				if (redirectStdin && inputLength > 0)
				{
					size_t numBytesWritten;
					while (WriteToPipe(newInput[1], input, inputLength, &numBytesWritten))
					{
						input		+= numBytesWritten;
						inputLength -= numBytesWritten;

						if (!inputLength)
							CloseSysHandle(newInput[1]);
					}
				}


				// ... stdout
				if (redirectStdout)
				{
					while (ReadFromPipe(newOutput[0], buffer, sizeof(buffer), &numBytesRead))
						output.append(buffer, numBytesRead);
				}

				// ... stderr
				if (redirectStderr)
				{
					while (ReadFromPipe(newError[0], buffer, sizeof(buffer), &numBytesRead))
						error.append(buffer, numBytesRead);
				}
			} while (WaitForSingleObject(pi.hProcess, 1000) != WAIT_OBJECT_0);

			// Sometimes the child process closes before its output appears in the pipes -- try to read it now
			if (redirectStdout)
			{
				while (ReadFromPipe(newOutput[0], buffer, sizeof(buffer), &numBytesRead))
					output.append(buffer, numBytesRead);
			}

			if (redirectStderr)
			{
				while (ReadFromPipe(newError[0], buffer, sizeof(buffer), &numBytesRead))
					error.append(buffer, numBytesRead);
			}

			// Save child process' output
			if (redirectStdout)
				SetSqString("OUTPUT", output.c_str());
			if (redirectStderr)
				SetSqString("ERROR", error.c_str());
		}

		DWORD exitCode;
		res = GetExitCodeProcess(pi.hProcess, &exitCode) ? SQInteger(exitCode) : -1;
		CloseHandle(pi.hProcess);
	}
	else
		PrintError("ERROR: Failed to run '%s'.\n", path.c_str());
#else // SHELL_PLATFORM_WINDOWS
	if (redirectStdin && pipe(newInput))
	{
		PrintError("ERROR: Failed to create input pipe.\n");
		goto finish;
	}

	if (redirectStdout && pipe(newOutput))
	{
		PrintError("ERROR: Failed to create output pipe.\n");
		goto finish;
	}

	if (redirectStderr && pipe(newError))
	{
		PrintError("ERROR: Failed to create error pipe.\n");
		goto finish;
	}

	pid_t pid;
	pid = fork();
	if (pid < 0)
		PrintError("ERROR: Failed to fork.\n");
	else if (pid == 0)
	{
		// Close unused pipe ends, duplicate used ones and run the program
		if (redirectStdin)
		{
			dup2(newInput[0], fileno(stdin));
			CloseSysHandle(newInput[1]);
		}
		if (redirectStdout)
		{
			CloseSysHandle(newOutput[0]);
			dup2(newOutput[1], fileno(stdout));
		}
		if (redirectStderr)
		{
			CloseSysHandle(newError[0]);
			dup2(newError[1], fileno(stderr));
		}

		execvp(path.c_str(), args);
		// If we got here, execvp() failed.
		PrintError("ERROR: Failed to run '%s'.\n", path.c_str());
		_exit(EXIT_FAILURE);
	}
	else
	{
		int childExitStatus = -1;
		if (!redirectStdin && !redirectStdout && !redirectStderr)
		{
			// Wait until the child process terminates
			waitpid(pid, &childExitStatus, 0);
		}
		else
		{
			// Close unused pipe ends and make used ones non-blocking
			if (redirectStdin)
			{
				CloseSysHandle(newInput[0]);
				fcntl(newInput[1], F_SETFL, fcntl(newInput[1], F_GETFL) | O_NONBLOCK);
			}
			if (redirectStdout)
			{
				fcntl(newOutput[0], F_SETFL, fcntl(newOutput[0], F_GETFL) | O_NONBLOCK);
				CloseSysHandle(newOutput[1]);
			}
			if (redirectStderr)
			{
				fcntl(newError[0], F_SETFL, fcntl(newError[0], F_GETFL) | O_NONBLOCK);
				CloseSysHandle(newError[1]);
			}

			// Pass data to/from child process' streams
			std::basic_string<SQChar> output,
									  error;
			int						  nfds = max(newInput[1], max(newOutput[0], newError[0])) + 1;
			for (;;)
			{
				// Check if there's any data available for reading/writing
				fd_set fdread;
				FD_ZERO(&fdread);
				if (redirectStdout)
					FD_SET(newOutput[0], &fdread);
				if (redirectStderr)
					FD_SET(newError[0], &fdread);

				fd_set fdwrite;
				FD_ZERO(&fdwrite);
				if (redirectStdin && inputLength > 0)
					FD_SET(newInput[1], &fdwrite);

				timeval timeout;
				timeout.tv_sec	= 1;
				timeout.tv_usec = 0;

				if (select(nfds, &fdread, &fdwrite, NULL, &timeout) < 0)
				{
					_RPT0(_CRT_WARN, "select() failed\n");
					waitpid(pid, &childExitStatus, 0);
					break;
				}

				// ... stdin
				if (FD_ISSET(newInput[1], &fdwrite))
				{
					size_t numBytesWritten;
					while (WriteToPipe(newInput[1], input, inputLength, &numBytesWritten))
					{
						input		+= numBytesWritten;
						inputLength -= numBytesWritten;

						if (!inputLength)
							CloseSysHandle(newInput[1]);
					}
				}

				SQChar buffer[PIPE_BUFFER_SIZE];
				size_t numBytesRead;

				// ... stdout
				if (FD_ISSET(newOutput[0], &fdread))
				{
					while (ReadFromPipe(newOutput[0], buffer, sizeof(buffer), &numBytesRead))
						output.append(buffer, numBytesRead);
				}

				// ... stderr
				if (FD_ISSET(newError[0], &fdread))
				{
					while (ReadFromPipe(newError[0], buffer, sizeof(buffer), &numBytesRead))
						error.append(buffer, numBytesRead);;
				}

				// Stop extracting data if the process has terminated
				waitpid(pid, &childExitStatus, WNOHANG);
				if (WIFEXITED(childExitStatus) || WIFSIGNALED(childExitStatus))
					break;
			}

			// Save child process' output
			if (redirectStdout)
				SetSqString("OUTPUT", output.c_str());
			if (redirectStderr)
				SetSqString("ERROR", error.c_str());
		}

		res = WIFEXITED(childExitStatus) ? WEXITSTATUS(childExitStatus) : -1;
	}

#endif // SHELL_PLATFORM_WINDOWS

finish:
	if (sq_gettype(sqvm, 3) == OT_ARRAY)
	{
		for (i = 1; i < numArgs; ++i)
			free(args[i]);
	}

	// Close pipes
	if (redirectStdin)
	{
		CloseSysHandle(newInput[0]);
		CloseSysHandle(newInput[1]);
	}
	if (redirectStdout)
	{
		CloseSysHandle(newOutput[0]);
		CloseSysHandle(newOutput[1]);
	}
	if (redirectStderr)
	{
		CloseSysHandle(newError[0]);
		CloseSysHandle(newError[1]);
	}

	sq_pushinteger(sqvm, res);
	return 1;
}

// chmod(string path, integer mode)
// Set file's permissions
static SQInteger ChMod (HSQUIRRELVM)
{
#if !defined(SHELL_PLATFORM_WINDOWS)
	const SQChar* path;
	SQInteger	  mode;
	sq_getstring(sqvm, 2, &path);
	sq_getinteger(sqvm, 3, &mode);
	chmod(ConvPath(path, SQTrue), mode_t(mode));
#endif
	return 0;
}

// chown(string path, string newOwner)
// Set file's owner
static SQInteger ChOwn (HSQUIRRELVM)
{
#if !defined(SHELL_PLATFORM_WINDOWS)
	const SQChar *path, *newOwner;
	sq_getstring(sqvm, 2, &path);
	sq_getstring(sqvm, 3, &newOwner);
	passwd* pwd = getpwnam(newOwner);
	if (pwd)
		chown(ConvPath(path, SQTrue), pwd->pw_uid, gid_t(-1));
#endif
	return 0;
}

// chgrp(string path, string newGroup)
// Set file's group
static SQInteger ChGrp (HSQUIRRELVM)
{
#if !defined(SHELL_PLATFORM_WINDOWS)
	const SQChar *path, *newGroup;
	sq_getstring(sqvm, 2, &path);
	sq_getstring(sqvm, 3, &newGroup);
	group* grp = getgrnam(newGroup);
	if (grp)
		chown(ConvPath(path, SQTrue), uid_t(-1), grp->gr_gid);
#endif
	return 0;
}

// string_array readdir(string path)
// Get list of directory entries
static SQInteger ReadDir (HSQUIRRELVM)
{
	const SQChar* path;
	sq_getstring(sqvm, 2, &path);

	SQChar dirPath[MAX_PATH];
	strncpy_s(dirPath, sizeof(dirPath), ConvPath(path, SQFalse), MAX_PATH);

	// Remove redundant trailing slashes
	size_t pathLen = strlen(dirPath);
	if (pathLen)
	{
		for (SQChar* c = &dirPath[pathLen - 1]; c > dirPath && c[0] == '/' && c[-1] == '/'; --c)
		{
			--pathLen;
			*c = 0;
		}
	}
	else
	{
		sq_pushnull(sqvm);
		return 1;
	}

	// Add trailing slash if necessary
	if (dirPath[pathLen - 1] != '/')
	{
		dirPath[pathLen++] = '/';
		dirPath[pathLen]   = 0;
	}

#if defined(SHELL_PLATFORM_WINDOWS)
	// Treat "/" path as system root
	if (dirPath[0] == '/')
	{
		SQChar winDir[MAX_PATH];
		if (GetWindowsDirectory(winDir, MAX_PATH))
			dirPath[0] = winDir[0];
		else
			dirPath[0] = 'c';
		dirPath[1] = ':';
		dirPath[2] = 0;
	}
#endif

	// Accept only existing directories
	bool isDir = false;
#if defined(SHELL_PLATFORM_WINDOWS)
	DWORD attrs = GetFileAttributes(ConvPath(dirPath, SQTrue));
	isDir = (attrs != INVALID_FILE_ATTRIBUTES) && (attrs & FILE_ATTRIBUTE_DIRECTORY);
#else
	struct stat fileStat;
	if (!stat(dirPath, &fileStat))
		isDir = S_ISDIR(fileStat.st_mode);
#endif
	if (!isDir)
	{
		sq_pushnull(sqvm);
		return 1;
	}

	SQChar tmp[MAX_PATH];
#if defined(SHELL_PLATFORM_WINDOWS)
	// Append wildcard to the end of path
#if (_MSC_VER >= 1400)
	_snprintf_s(tmp, MAX_PATH, MAX_PATH - 1, "%s*", dirPath);
#else
	_snprintf(tmp, MAX_PATH - 1, "%s*", dirPath);
#endif

	// List files
	WIN32_FIND_DATA fd;
	HANDLE			fh = FindFirstFile(ConvPath(tmp, SQTrue), &fd);
	if (fh != INVALID_HANDLE_VALUE)
	{
		// First 2 entries will be "." and ".." - skip them
		FindNextFile(fh, &fd);

		sq_newarray(sqvm, 0);
		while (FindNextFile(fh, &fd))
		{
#if (_MSC_VER >= 1400)
			_snprintf_s(tmp, MAX_PATH, MAX_PATH - 1, "%s%s", dirPath, fd.cFileName);
#else
			_snprintf(tmp, MAX_PATH - 1, "%s%s", dirPath, fd.cFileName);
#endif
			sq_pushstring(sqvm, ConvPath(tmp, SQFalse), -1);
			sq_arrayappend(sqvm, -2);
		}
		FindClose(fh);
	}
	else
		sq_pushnull(sqvm);
#else // SHELL_PLATFORM_WINDOWS
	DIR* dir = opendir(dirPath);
	if (dir)
	{
		// As in Windows, first 2 entries are "." and ".."
		readdir(dir);
		readdir(dir);

		dirent* de;
		sq_newarray(sqvm, 0);
		while ((de = readdir(dir)) != NULL)
		{
			snprintf(tmp, MAX_PATH - 1, "%s%s", dirPath, de->d_name);
			sq_pushstring(sqvm, ConvPath(tmp, SQFalse), -1);
			sq_arrayappend(sqvm, -2);
		}
		closedir(dir);
	}
	else
		sq_pushnull(sqvm);
#endif // SHELL_PLATFORM_WINDOWS
	return 1;
}

// exit([exitCode])
// Quit the shell
static SQInteger Exit (HSQUIRRELVM)
{
	SQInteger code = EXIT_SUCCESS;
	if (sq_gettop(sqvm) == 2)
		sq_getinteger(sqvm, 2, &code);

	retCode = int(code);
	return sq_suspendvm(sqvm);
}

// integer filetime(string path[, integer which = MODIFICATION])
// Get file time
// Supported values for 'which' parameter:
//    CREATION     - time when the file was created;
//    ACCESS       - time when the file was last accessed;
//    MODIFICATION - time when the file contents were modified;
//    CHANGE       - time when the file information (owner, group, etc.) was changed.
// Returned time is UTC
static SQInteger FileTime (HSQUIRRELVM)
{
	SQInteger	  res = 0;
	const SQChar* path;
	SQInteger	  which = FILETIME_CREATE;
	sq_getstring(sqvm, 2, &path);
	if (sq_gettop(sqvm) == 3)
		sq_getinteger(sqvm, 3, &which);

#if defined(SHELL_PLATFORM_WINDOWS)
	HANDLE file = CreateFile(ConvPath(path, SQTrue), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	if (file != INVALID_HANDLE_VALUE)
	{
		FILETIME cFTime, // Creation time
				 aFTime, // Last access time
				 mFTime; // Last modification time
		GetFileTime(file, &cFTime, &aFTime, &mFTime);
		CloseHandle(file);

		FILETIME* fileTime;
		switch (which)
		{
		case FILETIME_CREATE:
			fileTime = &cFTime;
			break;

		case FILETIME_ACCESS:
			fileTime = &aFTime;
			break;

		default:
			fileTime = &mFTime;
		}

		SYSTEMTIME sysTime;
		FileTimeToSystemTime(fileTime, &sysTime);
		res = TimeToInt(sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
	}
#else // SHELL_PLATFORM_WINDOWS
	struct stat fileStat;
	if (!stat(ConvPath(path, SQTrue), &fileStat))
	{
		time_t* fileTime;
		switch (which)
		{
		case FILETIME_CREATE:
		case FILETIME_ACCESS:
			fileTime = &fileStat.st_atime;
			break;

		case FILETIME_CHANGE:
			fileTime = &fileStat.st_ctime;
			break;

		default:
			fileTime = &fileStat.st_mtime;
		}
		tm* sysTime = gmtime(fileTime);
		res = TimeToInt(sysTime->tm_year + 1900, sysTime->tm_mon + 1, sysTime->tm_mday,
						sysTime->tm_hour, sysTime->tm_min, sysTime->tm_sec);
	}
#endif // SHELL_PLATFORM_WINDOWS
	sq_pushinteger(sqvm, res);
	return 1;
}

// integer systime()
// Get system time
static SQInteger SysTime (HSQUIRRELVM)
{
#if defined(SHELL_PLATFORM_WINDOWS)
	SYSTEMTIME sysTime;
	GetSystemTime(&sysTime);
	sq_pushinteger(sqvm, TimeToInt(sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond));
#else
	time_t curTime = time(NULL);
	tm*    sysTime = gmtime(&curTime);
	sq_pushinteger(sqvm, TimeToInt(sysTime->tm_year + 1900, sysTime->tm_mon + 1, sysTime->tm_mday,
								   sysTime->tm_hour, sysTime->tm_min, sysTime->tm_sec));
#endif
	return 1;
}

// integer localtime()
// Get system time
static SQInteger LocalTime (HSQUIRRELVM)
{
#if defined(SHELL_PLATFORM_WINDOWS)
	SYSTEMTIME localTime;
	GetLocalTime(&localTime);
	sq_pushinteger(sqvm, TimeToInt(localTime.wYear, localTime.wMonth, localTime.wDay,
								   localTime.wHour, localTime.wMinute, localTime.wSecond));
#else
	time_t curTime	 = time(NULL);
	tm*    localTime = localtime(&curTime);
	sq_pushinteger(sqvm, TimeToInt(localTime->tm_year + 1900, localTime->tm_mon + 1, localTime->tm_mday,
								   localTime->tm_hour, localTime->tm_min, localTime->tm_sec));
#endif
	return 1;
}

// setfiletime(string path, integer which, integer time)
// Modify file timestamp
static SQInteger SetFileTime2 (HSQUIRRELVM)
{
	const SQChar* path;
	SQInteger	  which,
				  timeValue;
	sq_getstring(sqvm, 2, &path);
	sq_getinteger(sqvm, 3, &which);
	sq_getinteger(sqvm, 4, &timeValue);

	// Unpack time
	unsigned year,
			 month,
			 day,
			 hour,
			 minute,
			 second;
	IntToTime(timeValue, year, month, day, hour, minute, second);

#if defined(SHELL_PLATFORM_WINDOWS)
	HANDLE file = CreateFile(ConvPath(path, SQTrue), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	if (file != INVALID_HANDLE_VALUE)
	{
		FILETIME cFTime,	// Creation time
				 aFTime,	// Last access time
				 mFTime;	// Last modification time
		GetFileTime(file, &cFTime, &aFTime, &mFTime);

		FILETIME   newFTime;
		SYSTEMTIME sysTime = { WORD(year), WORD(month), 0, WORD(day), WORD(hour), WORD(minute), WORD(second), 0 };
		SystemTimeToFileTime(&sysTime, &newFTime);
		switch (which)
		{
		case FILETIME_CREATE:
			SetFileTime(file, &newFTime, &aFTime, &mFTime);
			break;

		case FILETIME_ACCESS:
			SetFileTime(file, &cFTime, &newFTime, &mFTime);
			break;

		case FILETIME_MODIFY:
		case FILETIME_CHANGE:
			SetFileTime(file, &cFTime, &aFTime, &newFTime);
		}
		CloseHandle(file);
	}
#else // SHELL_PLATFORM_WINDOWS
	struct stat	  fileStat;
	const SQChar* filePath = ConvPath(path, SQTrue);
	if (!stat(filePath, &fileStat))
	{
		tm		sysTime  = { int(second), int(minute), int(hour), int(day), int(month - 1), int(year - 1900), 0, 0, 0 };
		time_t	newTime  = mktime(&sysTime);
		utimbuf newFTime = { fileStat.st_atime, fileStat.st_mtime };
		switch (which)
		{
		case FILETIME_CREATE:
		case FILETIME_ACCESS:
			newFTime.actime = newTime;
			break;

		case FILETIME_MODIFY:
		case FILETIME_CHANGE:
			newFTime.modtime = newTime;
			break;

		default:
			return 0;
		}
		utime(filePath, &newFTime);
	}
#endif // SHELL_PLATFORM_WINDOWS
	return 0;
}

void Init_Base ()
{
	SetSqInteger("CREATION", FILETIME_CREATE, SQTrue);
	SetSqInteger("ACCESS", FILETIME_ACCESS, SQTrue);
	SetSqInteger("MODIFICATION", FILETIME_MODIFY, SQTrue);
	SetSqInteger("CHANGE", FILETIME_CHANGE, SQTrue);
	SetSqString("OUTPUT", "");
	SetSqString("ERROR", "");
	SetSqFunction("mkdir", MkDir, -2, ". s i");
	SetSqFunction("getcwd", GetCWD, 1, ".");
	SetSqFunction("chdir", ChDir, 2, ". s");
	SetSqFunction("exist", Exist, 2, ". s");
	SetSqFunction("copy", Copy, -3, ". s s b");
	SetSqFunction("move", Move, 3, ". s s");
	SetSqFunction("remove", Remove, 2, ". s");
	SetSqFunction("rmdir", RmDir, -2, ". s b");
	SetSqFunction("getenv", GetEnv, 2, ". s");
	SetSqFunction("setenv", SetEnv, 3, ". s s");
	SetSqFunction("delenv", DelEnv, 2, ". s");
	SetSqFunction("run", Run, -2, ". s a|o|s o|s b b");
	SetSqFunction("chmod", ChMod, 3, ". s i");
	SetSqFunction("chown", ChOwn, 3, ". s s");
	SetSqFunction("chgrp", ChGrp, 3, ". s s");
	SetSqFunction("readdir", ReadDir, 2, ". s");
	SetSqFunction("exit", Exit, -1, ". i");
	SetSqFunction("filetime", FileTime, -2, ". s i");
	SetSqFunction("systime", SysTime, 1, ".");
	SetSqFunction("localtime", LocalTime, 1, ".");
	SetSqFunction("setfiletime", SetFileTime2, 4, ". s i i");

	_RPT0(_CRT_WARN, "------ Base library initialized\n");
}

void Shutdown_Base ()
{
	_RPT0(_CRT_WARN, "------ Base library deinitialized\n");
}
