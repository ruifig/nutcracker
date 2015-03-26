// Squirrel Shell
// Copyright (c) 2006-2009, Constantin Makshin
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
#include <ctype.h>

#if defined(SHELL_PLATFORM_WINDOWS)
//#	include <shlobj.h>
#else
#	include <sys/stat.h>
#	include <sys/types.h>
#	include <time.h>
#endif

#define MIN_YEAR  1980					// Minimum possible year
#define MAX_YEAR  2068					// Maximum possible year
#define NUM_YEARS (MAX_YEAR - MIN_YEAR) // Range for year values

#define FILETYPE_DIR  'd'
#define FILETYPE_FILE 'f'

// Date and time structure
union DateAndTime
{
	struct
	{
		SQUnsignedInteger time : 17;
		SQUnsignedInteger date : 15;
	} dt;
	SQInteger value;
};

// Register integer value in Squirrel
void SetSqInteger (const SQChar* name, SQInteger value, SQBool isConst)
{
	if (isConst)
		sq_pushconsttable(sqvm);
	else
		sq_pushroottable(sqvm);

	sq_pushstring(sqvm, name, -1);
	sq_pushinteger(sqvm, value);
	if (SQ_FAILED(sq_newslot(sqvm, -3, SQFalse)))
		PrintError("SQUIRREL ERROR: Failed to create \"%s\" integer value.\n", name);
	sq_pop(sqvm, 1);
}

// Register floating point value in Squirrel
void SetSqFloat (const SQChar* name, SQFloat value, SQBool isConst)
{
	if (isConst)
		sq_pushconsttable(sqvm);
	else
		sq_pushroottable(sqvm);

	sq_pushstring(sqvm, name, -1);
	sq_pushfloat(sqvm, value);
	if (SQ_FAILED(sq_newslot(sqvm, -3, SQFalse)))
		PrintError("SQUIRREL ERROR: Failed to create \"%s\" floating point value.\n", name);
	sq_pop(sqvm, 1);
}

// Register string value in Squirrel
void SetSqString (const SQChar* name, const SQChar* value, SQBool isConst)
{
	if (isConst)
		sq_pushconsttable(sqvm);
	else
		sq_pushroottable(sqvm);

	sq_pushstring(sqvm, name, -1);
	sq_pushstring(sqvm, value, -1);
	if (SQ_FAILED(sq_newslot(sqvm, -3, SQFalse)))
		PrintError("SQUIRREL ERROR: Failed to create \"%s\" string value.\n", name);
	sq_pop(sqvm, 1);
}

// Register C function in Squirrel
void SetSqFunction (const SQChar* name, SQFUNCTION func, SQInteger numParams, const SQChar* params)
{
	sq_pushroottable(sqvm);
	sq_pushstring(sqvm, name, -1);
	sq_newclosure(sqvm, func, 0);
	sq_setparamscheck(sqvm, numParams, params);
	if (SQ_FAILED(sq_newslot(sqvm, -3, SQFalse)))
		PrintError("SQUIRREL ERROR: Failed to create \"%s\" function.\n", name);
	sq_pop(sqvm, 1);
}

// Get number of days in month.
static unsigned NumberOfDays (unsigned month, unsigned year)
{
	static unsigned monthDays[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
	bool			leapYear	= (!(year % 4) && (year % 100)) || !(year % 400);
	return (leapYear && (month == 2)) ? 29 : monthDays[month - 1];
}

// Convert date and time to 32-bit integer number
// Date should be between Jan 1, MIN_YEAR and Feb 1, MAX_YEAR
// Time should be between 0:0:0 AM and 11:59:59 PM
// Values outside specified range are clamped
SQInteger TimeToInt (unsigned year, unsigned month, unsigned day, unsigned hour, unsigned minute, unsigned second)
{
	if (year >= MIN_YEAR)
		year -= MIN_YEAR;
	if (month)
		--month;
	if (day)
		--day;

	DateAndTime result;
	result.dt.time = (min(hour, 23) * 3600) + (min(minute, 59) * 60) + min(second, 59);
	result.dt.date = (min(year, NUM_YEARS) * 372) + (min(month, 11) * 31) + min(day, NumberOfDays(month, year) - 1);
	return result.value;
}

// Convert date and time from 32-bit integer number
void IntToTime (SQInteger value, unsigned& year, unsigned& month, unsigned& day, unsigned& hour, unsigned& minute, unsigned& second)
{
	DateAndTime dateAndTime;
	dateAndTime.value = value;

	// Convert date
	year = unsigned(dateAndTime.dt.date / 372);
	unsigned tmp = unsigned(dateAndTime.dt.date - year * 372);
	month = tmp / 31;
	day   = tmp - month * 31;
	++day;
	++month;
	year += MIN_YEAR;

	// Convert time
	hour   = unsigned(dateAndTime.dt.time / 3600);
	tmp    = unsigned(dateAndTime.dt.time - hour * 3600);
	minute = tmp / 60;
	second = tmp - minute * 60;
}

// Convert path to OS's native format (toNative = true)or from OS's native format to *nix format (toNative = false)
// Paths longer than MAX_PATH characters will be truncated
// In Windows tilde as the first character in path is replaced with current user's directory (c:\Documents and Settings\*)
const SQChar* ConvPath (const SQChar* path, SQBool toNative)
{
	static SQChar pth[MAX_PATH];
	SQChar		  *src	= (SQChar*)path,
				  *dest = pth;
	size_t		  n		= 0;
	if (path)
	{
		// Skip whitespace
		while (isspace(*src))
			++src;

#if defined(SHELL_PLATFORM_WINDOWS)
		if (toNative && (*src == '~'))
		{
#if 0
			if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_PROFILE, NULL, SHGFP_TYPE_CURRENT, pth)))
			{
#else
			// This should resolve strange issue with Windows NT 4.0 SP6 (missing SHGetFolderPath), but may break
			// Windows 9x compatibility... Dunno...
			DWORD res = GetEnvironmentVariable("USERPROFILE", pth, MAX_PATH);
			if (res && (res <= MAX_PATH))
			{
#endif
				n	 =  strlen(pth);
				dest += n;
			}
			else
			{
				// If failed to get the path, set it to "c:"
				*dest++ = 'c';
				*dest++ = ':';
				n		= 2;
			}
			++src;
		}
#endif
		for (; *src && (n < (MAX_PATH)); ++src, ++dest, ++n)
		{
			if ((*src == '/') || (*src == '\\'))
			{
#if defined(SHELL_PLATFORM_WINDOWS)
				if (toNative)
					*dest = '\\';
				else
#endif
					*dest = '/';
			}
			else
				*dest = *src;
		}
	}

	// Put terminating character
	*dest-- = 0;

	// Remove trailing whitespace
	while (isspace(*dest))
		*dest-- = 0;
	return pth;
}

// Get file type:
//    FILETYPE_DIR - directory;
//    FILETYPE_FILE - file.
static SQInteger FileType (const SQChar* path)
{
	if (!path)
		return 0;

	SQInteger res = 0;
#if defined(SHELL_PLATFORM_WINDOWS)
	DWORD attrs = GetFileAttributes(ConvPath(path, SQTrue));
	if (attrs != INVALID_FILE_ATTRIBUTES)
		res = (attrs & FILE_ATTRIBUTE_DIRECTORY) ? FILETYPE_DIR : FILETYPE_FILE;
#else
	struct stat fileStat;
	if (!stat(ConvPath(path, SQTrue), &fileStat))
	{
		if (S_ISDIR(fileStat.st_mode))
			res = FILETYPE_DIR;
		else if (S_ISREG(fileStat.st_mode) || S_ISLNK(fileStat.st_mode))
			res = FILETYPE_FILE;
	}
#endif
	return res;
}

// string convpath(string path, bool toNative)
// Convert path to OS's native format (toNative = true)or from OS's native format to *nix format (toNative = false)
static SQInteger ConvPath2 (HSQUIRRELVM)
{
	const SQChar* path;
	SQBool		  toNative;
	sq_getstring(sqvm, 2, &path);
	sq_getbool(sqvm, 3, &toNative);
	sq_pushstring(sqvm, ConvPath(path, toNative), -1);
	return 1;
}

// string pathenv(string path1[, string path2[, ...]])
// Build PATH environment variable
static SQInteger PathEnv (HSQUIRRELVM)
{
	SQInteger top = sq_gettop(sqvm) + 1,
			  i;

	// All parameters must be strings (empty strings are ignored)
	for (i = 2; i < top; ++i)
	{
		if (sq_gettype(sqvm, i) != OT_STRING)
			return sq_throwerror(sqvm, "all arguments of pathenv() function must be strings");
	}

	SQChar* buf		  = NULL;
	size_t	totalSize = 0;
	for (i = 2; i < top; ++i)
	{
		SQInteger thisSize = sq_getsize(sqvm, i);
		if (!thisSize)
			continue;

		totalSize += thisSize + 2;
		buf		  =  (SQChar*)realloc(buf, totalSize);
		if (!buf)
		{
			PrintError("ERROR: Not enough memory.\n");
			break;
		}

		const SQChar* path;
		sq_getstring(sqvm, i, &path);
		if (i == 2)
			memset(buf, 0, totalSize);
		else
		{
#if defined(SHELL_PLATFORM_WINDOWS)
			strcat_s(buf, totalSize, ";");
#else
			strcat(buf, ":");
#endif
		}
		strcat_s(buf, totalSize, path);
	}
	if (buf)
	{
		sq_pushstring(sqvm, ConvPath(buf, SQTrue), -1);
		free(buf);
	}
	else
		sq_pushnull(sqvm);
	return 1;
}

// string platform()
// Get name of OS shell was compiled in
static SQInteger Platform (HSQUIRRELVM)
{
	sq_pushstring(sqvm, SHELL_PLATFORM, -1);
	return 1;
}

// string cpuarch()
// Get CPU architecture shell was compiled on
static SQInteger CPUArch (HSQUIRRELVM)
{
	sq_pushstring(sqvm, SHELL_CPUARCH, -1);
	return 1;
}

// integer strchar(string str, integer i)
// Get i'th character of string
static SQInteger StrChar (HSQUIRRELVM)
{
	SQInteger	  res = 0;
	const SQChar* str;
	SQInteger	  i;
	sq_getstring(sqvm, 2, &str);
	sq_getinteger(sqvm, 3, &i);

	if ((i >= 0) && (i < sq_getsize(sqvm, 2)))
		res = str[i];
	sq_pushinteger(sqvm, res);
	return 1;
}

// string substr(string str, integer a, integer b)
// Get substring that begins at a'th character and ends at b'th one
static SQInteger SubStr (HSQUIRRELVM)
{
	bool		  ok = false;
	const SQChar* str;
	SQInteger	  a, b;
	sq_getstring(sqvm, 2, &str);
	sq_getinteger(sqvm, 3, &a);
	sq_getinteger(sqvm, 4, &b);

	SQInteger len = sq_getsize(sqvm, 2);
	if ((a >= 0) && (a <= len) && (b >= a) && (b <= len))
	{
		sq_pushstring(sqvm, str + a, b - a);
		ok = true;
	}
	if (!ok)
		sq_pushnull(sqvm);
	return 1;
}

// string filepath(string path)
// Get path to file (everything before file's name)
// Returned path doesn't include trailing (back)slash
static SQInteger FilePath (HSQUIRRELVM)
{
	const SQChar* path;
	sq_getstring(sqvm, 2, &path);

	const SQChar* buf = ConvPath(path, SQFalse);
	SQChar*		  end = (SQChar*)strrchr(buf, '/');
	if (end)
		sq_pushstring(sqvm, path, SQInteger(size_t(end) - size_t(buf)));
	else
		sq_pushstring(sqvm, ".", -1);
	return 1;
}

// string filename(string path[, bool ext = false])
// Get name of file with (ext = true) or without (ext = false) extension
// Returned name doesn't include leading (back)slash (in any case) nor period separating name and extension (ext = false)
static SQInteger FileName (HSQUIRRELVM)
{
	const SQChar* path;
	SQBool		  ext = SQFalse;
	sq_getstring(sqvm, 2, &path);
	if (sq_gettop(sqvm) == 3)
		sq_getbool(sqvm, 3, &ext);

	const SQChar* buf	  = ConvPath(path, SQFalse);
	SQChar*		  pathEnd = (SQChar*)strrchr(buf, '/');
	SQInteger	  n		  = -1;
	if (pathEnd)
		++pathEnd;
	else
		pathEnd = (SQChar*)buf;

	if (!ext)
	{
		SQChar* nameEnd = strchr(pathEnd, '.');
		// Treat names like ".boo" as names without extension
		if ((nameEnd > buf) && (nameEnd[-1] != '/'))
			n = SQInteger(size_t(nameEnd) - size_t(pathEnd));
	}

	sq_pushstring(sqvm, pathEnd, n);
	return 1;
}

// string fileext(string path)
// Get file's extension
// Returned extension doesn't include leading period
static SQInteger FileExt (HSQUIRRELVM)
{
	const SQChar* path;
	sq_getstring(sqvm, 2, &path);

	SQChar* end = (SQChar*)strrchr(path, '.');

	// Treat names like ".boo" as names without extension
	if ((end == path) || ((end > path) && (end[-1] == '/')))
		end = NULL;

	sq_pushstring(sqvm, end ? (end + 1) : "", -1);
	return 1;
}

// integer filetype(string path)
// Get file type:
//    0    - the function failed;
//    DIR  - directory;
//    FILE - file.
static SQInteger FileType2 (HSQUIRRELVM)
{
	const SQChar* path;
	sq_getstring(sqvm, 2, &path);
	sq_pushinteger(sqvm, FileType(path));
	return 1;
}

// string strtime(integer time[, string format = "%b %d %T %Y"])
// Represent date and time in text format.
// Supported format fields:
//    %b - Abbreviated month name (Jan, Feb, etc.)
//    %B - Full month name (January, February, etc.)
//    %C - Century
//    %d - Day of month (always two digits)
//    %D - Month/Day/Year (e.g., "03/08/89")
//    %e - Day of month (leading zero blanked)
//    %H - 24-hour-clock hour (two digits)
//    %h - 12-hour-clock hour (two digits)
//    %k - 12-hour-clock hour (leading zero blanked)
//    %l - 24-hour-clock hour (leading zero blanked)
//    %m - Month number (two digits)
//    %M - Minute (two digits)
//    %p - AM/PM designation
//    %r - Hour:Minute:Second AM/PM designation (e.g., "02:54:40 PM")
//    %R - Hour:Minute (e.g., "14:54")
//    %S - Second (two digits)
//    %T - Hour:Minute:Second (e.g., "14:54:40")
//    %y - Last two digits of year
//    %Y - Year in full
static SQInteger StrTime (HSQUIRRELVM)
{
	SQInteger	  intTime = 0;
	const SQChar* fmt	  = "%b %d %T %Y";
	sq_getinteger(sqvm, 2, &intTime);
	if (sq_gettop(sqvm) == 3)
		sq_getstring(sqvm, 3, &fmt);

	// Unpack time
	unsigned year,
			 month,
			 day,
			 hour,
			 minute,
			 second;
	IntToTime(intTime, year, month, day, hour, minute, second);

	// Set common values to avoid redundant branching
	unsigned	  hour12 = !hour || (hour == 12) ? 12 : (hour % 12); // 12-hour-clock hour
	const SQChar* amPm	 = (hour < 12) ? "AM" : "PM";				 // AM/PM designation

	// Format time
	static const SQChar *shortMonths[]	= { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov",
											"Dec" },
						*fullMonths[]	= { "January", "February", "March", "April", "May", "June", "July", "August",
											"September", "October", "November", "December" };
	static const size_t	monthNameLens[]	= { 7, 8, 5, 5, 3, 4, 4, 6, 9, 7, 8, 8 };
	const SQChar		*src			= fmt;
	SQChar				buf[1024],
						*dest			= buf;
	size_t				left			= sizeof(buf) - 1;
	for (; *src && left; ++src)
	{
		if (*src != '%')
		{
			*dest++ = *src;
			--left;
			continue;
		}

		// As there's no format field that could add less than two characters, end parsing if the result is getting too long
		if (left < 2)
			break;

		// Handle format fields
		size_t numChars = 2;
		switch (*++src)
		{
		case 'b':
			if (left < 3)
				goto pushresult;
			strncpy_s(dest, left + 1, shortMonths[month - 1], left + 1);
			numChars = 3;
			break;

		case 'B':
			numChars = monthNameLens[month - 1];
			if (left < numChars)
				goto pushresult;
			strncpy_s(dest, left + 1, fullMonths[month - 1], left + 1);
			break;

		case 'C':
#if defined(_MSC_VER) && (_MSC_VER >= 1400)
			sprintf_s(dest, left + 1, "%02u", year / 100);
#else
			sprintf(dest, "%02u", year / 100);
#endif
			break;

		case 'd':
#if defined(_MSC_VER) && (_MSC_VER >= 1400)
			sprintf_s(dest, left + 1, "%02u", day);
#else
			sprintf(dest, "%02u", day);
#endif
			break;

		case 'D':
			if (left < 8)
				goto pushresult;
#if defined(_MSC_VER) && (_MSC_VER >= 1400)
			sprintf_s(dest, left + 1, "%02u/%02u/%02u", month, day, year % 100);
#else
			sprintf(dest, "%02u/%02u/%02u", month, day, year % 100);
#endif
			numChars = 8;
			break;

		case 'e':
#if defined(_MSC_VER) && (_MSC_VER >= 1400)
			sprintf_s(dest, left + 1, "%2u", day);
#else
			sprintf(dest, "%2u", day);
#endif
			break;

		case 'H':
#if defined(_MSC_VER) && (_MSC_VER >= 1400)
			sprintf_s(dest, left + 1, "%02u", hour);
#else
			sprintf(dest, "%02u", hour);
#endif
			break;

		case 'h':
#if defined(_MSC_VER) && (_MSC_VER >= 1400)
			sprintf_s(dest, left + 1, "%02u", hour12);
#else
			sprintf(dest, "%02u", hour12);
#endif
			break;

		case 'k':
#if defined(_MSC_VER) && (_MSC_VER >= 1400)
			sprintf_s(dest, left + 1, "%2u", hour12);
#else
			sprintf(dest, "%2u", hour12);
#endif
			break;

		case 'l':
#if defined(_MSC_VER) && (_MSC_VER >= 1400)
			sprintf_s(dest, left + 1, "%2u", hour);
#else
			sprintf(dest, "%2u", hour);
#endif
			break;

		case 'm':
#if defined(_MSC_VER) && (_MSC_VER >= 1400)
			sprintf_s(dest, left + 1, "%02u", month);
#else
			sprintf(dest, "%02u", month);
#endif
			break;

		case 'M':
#if defined(_MSC_VER) && (_MSC_VER >= 1400)
			sprintf_s(dest, left + 1, "%02u", minute);
#else
			sprintf(dest, "%02u", minute);
#endif
			break;

		case 'p':
			strncpy_s(dest, left + 1, amPm, left + 1);
			break;

		case 'r':
			if (left < 11)
				goto pushresult;
#if defined(_MSC_VER) && (_MSC_VER >= 1400)
			sprintf_s(dest, left + 1, "%02u:%02u:%02u %s", hour12, minute, second, amPm);
#else
			sprintf(dest, "%02u:%02u:%02u %s", hour12, minute, second, amPm);
#endif
			numChars = 11;
			break;

		case 'R':
			if (left < 5)
				goto pushresult;
#if defined(_MSC_VER) && (_MSC_VER >= 1400)
			sprintf_s(dest, left + 1, "%02u:%02u", hour, minute);
#else
			sprintf(dest, "%02u:%02u", hour, minute);
#endif
			numChars = 5;
			break;

		case 'S':
#if defined(_MSC_VER) && (_MSC_VER >= 1400)
			sprintf_s(dest, left + 1, "%02u", second);
#else
			sprintf(dest, "%02u", second);
#endif
			break;

		case 'T':
			if (left < 8)
				goto pushresult;
#if defined(_MSC_VER) && (_MSC_VER >= 1400)
			sprintf_s(dest, left + 1, "%02u:%02u:%02u", hour, minute, second);
#else
			sprintf(dest, "%02u:%02u:%02u", hour, minute, second);
#endif
			numChars = 8;
			break;

		case 'y':
#if defined(_MSC_VER) && (_MSC_VER >= 1400)
			sprintf_s(dest, left + 1, "%02u", year % 100);
#else
			sprintf(dest, "%02u", year % 100);
#endif
			break;

		case 'Y':
			if (left < 4)
				goto pushresult;
#if defined(_MSC_VER) && (_MSC_VER >= 1400)
			sprintf_s(dest, left + 1, "%u", year);
#else
			sprintf(dest, "%u", year);
#endif
			numChars = 4;
			break;

		default:
			dest[0] = '%';
			dest[1] = *src;
		}
		dest += numChars;
		left -= numChars;
	}

pushresult:
	*dest = 0;
	sq_pushstring(sqvm, buf, -1);
	return 1;
}

// integer utctolocal(integer time)
// Convert time from UTC to local
static SQInteger UtcToLocal (HSQUIRRELVM)
{
	SQInteger sysTime;
	sq_getinteger(sqvm, 2, &sysTime);

	long offset = 0;
#if defined(SHELL_PLATFORM_WINDOWS)
	TIME_ZONE_INFORMATION tzi;
	if (GetTimeZoneInformation(&tzi) != TIME_ZONE_ID_INVALID)
		offset = -tzi.Bias / 60;
#else
	offset = -timezone / 3600;
#endif

	// Unpack time
	unsigned year,
			 month,
			 day,
			 hour,
			 minute,
			 second;
	IntToTime(sysTime, year, month, day, hour, minute, second);

	// Modify time and handle possible date changes
	hour += offset;
	if (hour > 23)
	{
		hour -= 24;
		++day;
	}
	if (day > NumberOfDays(month, year))
	{
		day = 1;
		++month;
	}
	if (month > 12)
	{
		month = 1;
		++year;
	}

	sq_pushinteger(sqvm, TimeToInt(year, month, day, hour, minute, second));
	return 1;
}

// integer localtoutc(integer time)
// Convert time from local to UTC
static SQInteger LocalToUtc (HSQUIRRELVM)
{
	SQInteger localTime;
	sq_getinteger(sqvm, 2, &localTime);

	long offset = 0;
#if defined(SHELL_PLATFORM_WINDOWS)
	TIME_ZONE_INFORMATION tzi;
	if (GetTimeZoneInformation(&tzi) != TIME_ZONE_ID_INVALID)
		offset = tzi.Bias / 60;
#else
	offset = timezone / 3600;
#endif

	// Unpack time
	unsigned year,
			 month,
			 day,
			 hour,
			 minute,
			 second;
	IntToTime(localTime, year, month, day, hour, minute, second);

	// Modify time and handle possible date changes
	hour += offset;
	if (hour < 0)
	{
		hour += 24;
		--day;
	}
	if (day < 0)
		--month;
	if (!month)
	{
		month = 12;
		--year;
	}
	if (day < 0)
		day = NumberOfDays(month, year);

	sq_pushinteger(sqvm, TimeToInt(year, month, day, hour, minute, second));
	return 1;
}

// integer mktime(integer year, integer month, integer day, integer hour, integer minute, integer second)
// Pack specified time
// Date should be between Jan 1, MIN_YEAR and Feb 1, MAX_YEAR
// Time should be between 0:0:0 AM and 11:59:59 PM
// Values outside specified range are clamped
static SQInteger MkTime (HSQUIRRELVM)
{
	SQInteger year,
			  month,
			  day,
			  hour,
			  minute,
			  second;
	sq_getinteger(sqvm, 2, &year);
	sq_getinteger(sqvm, 3, &month);
	sq_getinteger(sqvm, 4, &day);
	sq_getinteger(sqvm, 5, &hour);
	sq_getinteger(sqvm, 6, &minute);
	sq_getinteger(sqvm, 7, &second);
	sq_pushinteger(sqvm, TimeToInt(unsigned(max(year, MIN_YEAR)), unsigned(max(month, 1)), unsigned(max(day, 1)),
								   unsigned(max(hour, 0)), unsigned(max(minute, 0)), unsigned(max(second, 0))));
	return 1;
}

void Init_Util ()
{
#if !defined(SHELL_PLATFORM_WINDOWS)
	// We need to set 'timezone' variable
	tzset();
#endif

	SetSqInteger("DIR", FILETYPE_DIR, SQTrue);
	SetSqInteger("FILE", FILETYPE_FILE, SQTrue);
	SetSqFunction("convpath", ConvPath2, 3, ". s b");
	SetSqFunction("pathenv", PathEnv, -2, ". s");
	SetSqFunction("platform", Platform, 1, ".");
	SetSqFunction("cpuarch", CPUArch, 1, ".");
	SetSqFunction("strchar", StrChar, 3, ". s i");
	SetSqFunction("substr", SubStr, 4, ". s i i");
	SetSqFunction("filepath", FilePath, 2, ". s");
	SetSqFunction("filename", FileName, -2, ". s b");
	SetSqFunction("fileext", FileExt, 2, ". s");
	SetSqFunction("filetype", FileType2, 2, ". s");
	SetSqFunction("strtime", StrTime, -2, ". i s");
	SetSqFunction("utctolocal", UtcToLocal, 2, ". i");
	SetSqFunction("localtoutc", LocalToUtc, 2, ". i");
	SetSqFunction("mktime", MkTime, 7, ". i i i i i i");

	_RPT0(_CRT_WARN, "------ Utility library initialized\n");
}

void Shutdown_Util ()
{
	_RPT0(_CRT_WARN, "------ Utility library deinitialized\n");
}
