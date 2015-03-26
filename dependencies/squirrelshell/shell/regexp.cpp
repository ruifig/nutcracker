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
#include <pcre.h>

#define MAX_COMPILED_EXPRESSIONS 32   // Maximum number of simultaneously compiled regular expressions
#define MAX_ERROR_LENGTH		 4096 // Maximum length of error description
#define MAX_VERSION_LENGTH		 32   // Maximum length of PCRE version number

static pcre*	regExps[MAX_COMPILED_EXPRESSIONS];
static unsigned numRegExps					= 0;
static SQChar	lastError[MAX_ERROR_LENGTH] = { 0 }; // Description of last occured error

// Compile regular expression
static pcre* CompileRegExp (const char* regExp, int options)
{
	const char*	error;
	int			errOffset;
	pcre*		res = pcre_compile(regExp, options, &error, &errOffset, NULL);
	if (!res)
	{
#if defined(_MSC_VER)
#if (_MSC_VER >= 1400)
		_snprintf_s(lastError, MAX_ERROR_LENGTH, MAX_ERROR_LENGTH - 1, "%d: %s", errOffset, error);
#else
		_snprintf(lastError, MAX_ERROR_LENGTH - 1, "%d: %s", errOffset, error);
#endif // _MSC_VER >= 1400
#else // _MSC_VER
		snprintf(lastError, MAX_ERROR_LENGTH - 1, "%d: %s", errOffset, error);
#endif
	}
	return res;
}

// Get regular expression from Squirrel stack
static pcre* GetRegExp ()
{
	pcre* regExp;
	sq_getuserpointer(sqvm, 2, (SQUserPointer*)&regExp);
	if (regExp)
	{
		for (unsigned i = 0; i < MAX_COMPILED_EXPRESSIONS; ++i)
		{
			if (regExp == regExps[i])
				return regExp;
		}
	}
	return NULL;
}

// Free all compiled regular expressions
static void FreeAllRegExps ()
{
#if defined(_DEBUG)
	unsigned n = 0;
#endif
	for (unsigned i = 0; i < MAX_COMPILED_EXPRESSIONS; ++i)
	{
		if (regExps[i])
		{
			pcre_free(regExps[i]);
#if defined(_DEBUG)
			++n;
#endif
		}
		regExps[i] = NULL;
	}
	numRegExps = 0;
	_RPT1(_CRT_WARN, "--- FreeAllRegExps() freed %u expression(s)\n", n);
}

// Set lastError based on error code
static void SetLastError (int errCode)
{
	const SQChar* error;
	switch (errCode)
	{
	case 0:
		error = "no errors";
		break;

	case PCRE_ERROR_NOMATCH:
		error = "the subject string did not match the pattern";
		break;

	case PCRE_ERROR_NULL:
		error = "unexpected NULL parameter";
		break;

	case PCRE_ERROR_BADOPTION:
		error = "unknown option";
		break;

	case PCRE_ERROR_BADMAGIC:
		error = "invalid magic number";
		break;

	case PCRE_ERROR_UNKNOWN_NODE:
		error = "unknown item was encountered in the compiled pattern";
		break;

	case PCRE_ERROR_NOMEMORY:
		error = "not enough memory";
		break;

	case PCRE_ERROR_NOSUBSTRING:
		error = "no such substring";
		break;

	case PCRE_ERROR_MATCHLIMIT:
		error = "the backtracking limit was reached";
		break;

	case PCRE_ERROR_CALLOUT:
		error = "callout function error";
		break;

	case PCRE_ERROR_BADUTF8:
		error = "subject contains invalid UTF-8 byte sequence";
		break;

	case PCRE_ERROR_BADUTF8_OFFSET:
		error = "startOffset did not point to the beginning of a UTF-8 character";
		break;

	case PCRE_ERROR_PARTIAL:
		error = "the subject string matched only partially";
		break;

	case PCRE_ERROR_BADPARTIAL:
		error = "the PCRE_PARTIAL option was used with a compiled pattern containing items that are not supported for partial matching";
		break;

	case PCRE_ERROR_INTERNAL:
		error = "internal error";
		break;

	case PCRE_ERROR_BADCOUNT:
		error = "negative number of substrings";
		break;

	case PCRE_ERROR_RECURSIONLIMIT:
		error = "the internal recursion limit was reached";
		break;

	default:
		error = "unknown error";
	}
	strncpy_s(lastError, sizeof(lastError), error, MAX_ERROR_LENGTH);
}

// handle regcompile(string pattern[, bool caseSensitive = true[, bool multiLine = false]])
// Compile regular expression
static SQInteger RegCompile (HSQUIRRELVM)
{
	if (numRegExps == MAX_COMPILED_EXPRESSIONS)
	{
		PrintError("ERROR: Too many regular expressions.\n");
		sq_pushnull(sqvm);
		return 1;
	}

	// Find first free expression
	unsigned i;
	for (i = 0; i < MAX_COMPILED_EXPRESSIONS; ++i)
	{
		if (!regExps[i])
			break;
	}

	// Set compilation settings
	const SQChar* pattern;
	sq_getstring(sqvm, 2, &pattern);

	int options = 0;
	if (sq_gettop(sqvm) > 2)
	{
		SQBool caseSensitive = SQTrue;
		sq_getbool(sqvm, 3, &caseSensitive);
		if (!caseSensitive)
			options |= PCRE_CASELESS;
	}
	if (sq_gettop(sqvm) == 4)
	{
		SQBool multiLine = SQFalse;
		sq_getbool(sqvm, 4, &multiLine);
		if (multiLine)
			options |= PCRE_MULTILINE;
	}

	// Reset error
	SetLastError(0);

	// Compile expression
	pcre* res = regExps[i] = CompileRegExp(pattern, options);
	if (!res)
		sq_pushnull(sqvm);
	else
	{
		sq_pushuserpointer(sqvm, res);
		++numRegExps;
		_RPT2(_CRT_WARN, "--- Compiled expression #%d. %d expression(s) are compiled.\n", i, numRegExps);
	}
	return 1;
}

// array regmatch(handle regExp, string text[, integer startOffset = 0[, bool partial = false])
// array regmatch(string pattern, string text[, integer startOffset = 0[, bool partial = false])
// Match regular expression and return substrings
static SQInteger RegMatch (HSQUIRRELVM)
{
	// Reset error
	SetLastError(0);

	const SQChar* pattern = NULL;
	pcre*		  regExp  = NULL;
	if (sq_gettype(sqvm, 2) == OT_STRING)
	{
		if (SQ_SUCCEEDED(sq_getstring(sqvm, 2, &pattern)))
			regExp = CompileRegExp(pattern, 0);
	}
	else
		regExp = GetRegExp();

	if (!regExp)
	{
		strncpy_s(lastError, sizeof(lastError), "invalid regular expression", MAX_ERROR_LENGTH);
		sq_pushnull(sqvm);
	}
	else
	{
		// Get parameters
		const SQChar* text;
		SQInteger	  textLength = sq_getsize(sqvm, 3);
		sq_getstring(sqvm, 3, &text);

		SQInteger startOffset = 0;
		if (sq_gettop(sqvm) > 3)
			sq_getinteger(sqvm, 4, &startOffset);

		int options = 0;
		if (sq_gettop(sqvm) == 5)
		{
			SQBool partial = SQFalse;
			sq_getbool(sqvm, 5, &partial);
			if (partial)
				options |= PCRE_PARTIAL;
		}

		// Allocate memory for substrings
		int numSubStrings = 0;
		pcre_fullinfo(regExp, NULL, PCRE_INFO_CAPTURECOUNT, &numSubStrings);

		int	subStrVecSize = (numSubStrings + 1) * 3,
			*subStrings	  = (int*)malloc(subStrVecSize * sizeof(int));
		if (!subStrings)
		{
			SetLastError(PCRE_ERROR_NOMEMORY);
			sq_pushnull(sqvm);
			return 1;
		}

		// Perform matching
		int res = pcre_exec(regExp, NULL, text, int(textLength), int(startOffset), options, subStrings, subStrVecSize);
		if (res < 0)
		{
			// Delete regular expression if it was created here
			if (pattern)
				pcre_free(regExp);
			free(subStrings);
			SetLastError(res);
			sq_pushnull(sqvm);
			return 1;
		}

		// Put substrings to array
		sq_newarray(sqvm, 0);

		int* subStr = subStrings;
		while (res-- > 0)
		{
			sq_newarray(sqvm, 0);
			if (subStr[0] < 0)
				subStr[0] = 0;
			if (subStr[1] < 0)
				subStr[1] = subStr[0];
			sq_pushinteger(sqvm, subStr[0]);
			sq_arrayappend(sqvm, -2);
			sq_pushinteger(sqvm, subStr[1]);
			sq_arrayappend(sqvm, -2);
			sq_arrayappend(sqvm, -2);
			subStr += 2;
		}

		// Delete regular expression if it was created here
		if (pattern)
			pcre_free(regExp);
		free(subStrings);
	}
	return 1;
}

// regfree(handle regExp)
// Free resources used by regular expression
static SQInteger RegFree (HSQUIRRELVM)
{
	// Reset error
	SetLastError(0);

	pcre* regExp;
	sq_getuserpointer(sqvm, 2, (SQUserPointer*)&regExp);
	if (regExp)
	{
		for (unsigned i = 0; i < MAX_COMPILED_EXPRESSIONS; ++i)
		{
			if (regExp == regExps[i])
			{
				pcre_free(regExp);
				regExps[i] = NULL;
				--numRegExps;
				_RPT2(_CRT_WARN, "--- Freed expression #%d. %d expression(s) left.\n", i, numRegExps);
				break;
			}
		}
	}
	return 0;
}

// regfreeall()
// Free all regular expressions.
static SQInteger RegFreeAll (HSQUIRRELVM)
{
	// Reset error
	SetLastError(0);

	FreeAllRegExps();
	return 0;
}

// string regerror()
// Return description of last occured error.
static SQInteger RegError (HSQUIRRELVM)
{
	sq_pushstring(sqvm, lastError, -1);
	return 1;
}

void Init_RegExp ()
{
	for (unsigned i = 0; i < MAX_COMPILED_EXPRESSIONS; ++i)
		regExps[i] = NULL;

	memset(lastError, 0, sizeof(lastError));
	SetLastError(0);

	SQChar		pcreVersion[MAX_VERSION_LENGTH] = { 0 };
	const char*	pcreError;
	int			pcreErrOffset;
	pcre*		re = pcre_compile("\\d+\\.\\d+(\\.\\d+)?", 0, &pcreError, &pcreErrOffset, NULL);
	if (re)
	{
		const char*	versionStr = pcre_version();
		int			subStrings[12] = { -1, -1 };
		if (pcre_exec(re, NULL, versionStr, int(strlen(versionStr)), 0, 0, subStrings, 6) >= 0 &&
			 subStrings[0] >= 0 && subStrings[1] > 0 && subStrings[1] < MAX_VERSION_LENGTH)
		{
			strncpy_s(pcreVersion, sizeof(pcreVersion), versionStr + subStrings[0], subStrings[1]);
		}
		pcre_free(re);
	}
	SetSqString("PCRE_VERSION", pcreVersion, SQTrue);

	SetSqFunction("regcompile", RegCompile, -2, ". s b b");
	SetSqFunction("regmatch", RegMatch, -3, ". p|s s i b");
	SetSqFunction("regfree", RegFree, 2, ". p");
	SetSqFunction("regfreeall", RegFreeAll, 1, ".");
	SetSqFunction("regerror", RegError, 1, ".");

	_RPT0(_CRT_WARN, "------ RegExp library initialized\n");
}

void Shutdown_RegExp ()
{
	FreeAllRegExps();
	_RPT0(_CRT_WARN, "------ RegExp library deinitialized\n");
}
