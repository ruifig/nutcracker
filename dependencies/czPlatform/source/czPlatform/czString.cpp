/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com

	created:	2012/02/27
	created:	27:2:2012   14:58
	filename: 	czString.h
	
	purpose:	String related classes and code
*********************************************************************/

#include "czPlatformStableHeaders.h"
#include "czString.h"


namespace cz
{

bool notSpace(int a)
{
    return !(a==' ' || a=='\t' || a==0xA || a==0xD);
}

void splitString(const char* str, char delimiter, TArray<String> &tokens)
{
	tokens.clear();

	while(*str!=0)
	{
		// find a delimiter
		const char* ptrToChar = str;
		while(!(*str==0 || *str==delimiter))
			str++;

		size_t numchars = str-ptrToChar;
		tokens.push_back(String());
		tokens.last().append(ptrToChar, numchars);

		// skip delimiters
		while(*str!=0 && *str==delimiter)
			str++;
	}
}

void stringSplitIntoLines(const char* textbuffer, int buffersize, std::vector<String> *lines)
{
    lines->clear();
    if (*textbuffer ==0)
        return;

    const char *s = textbuffer;
    while(*s!=0 && s<textbuffer+buffersize)
    {
        const char* ptrToChar = s;
        while(!(*s==0 || *s==0xA || *s==0xD))
            s++;

        size_t numchars = s-ptrToChar;
        lines->push_back(String());
        lines->back().append(ptrToChar, ptrToChar+numchars);

        // New lines format are:
        // Unix		: 0xA
        // Mac		: 0xD
        // Windows	: 0xD 0xA
        // If windows format a new line has 0xD 0xA, so we need to skip one extra character
        if (*s==0xD && *(s+1)==0xA)
            s++;

        if (*s==0)
            break;

        s++; // skip the newline character
    }
}

char* getTemporaryString()
{
	// Use several static strings, and keep picking the next one, so that callers can hold the string for a while without risk of it
	// being changed by another call.
	__declspec( thread ) static char bufs[CZ_TEMPORARY_STRING_MAX_NESTING][CZ_TEMPORARY_STRING_MAX_SIZE];
	__declspec( thread ) static int nBufIndex=0;

	char* buf = bufs[nBufIndex];
	nBufIndex++;
	if (nBufIndex==CZ_TEMPORARY_STRING_MAX_NESTING)
		nBufIndex = 0;

	return buf;
}

const char* formatString(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	char *str= formatStringVA(format, args);
	va_end(args);
	return str;
}

char* formatStringVA(const char* format, va_list argptr)
{
	char* buf = getTemporaryString();
	if (_vsnprintf(buf, CZ_TEMPORARY_STRING_MAX_SIZE, format, argptr) == CZ_TEMPORARY_STRING_MAX_SIZE)
		buf[CZ_TEMPORARY_STRING_MAX_SIZE-1] = 0;
	return buf;
}

bool hasEnding(const String& str, const String& ending)
{
    if (str.length() >= ending.length()) {
        return (0 == str.compare (str.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
}

bool hasEnding(const std::string& str, const std::string& ending)
{
    if (str.length() >= ending.length()) {
        return (0 == str.compare (str.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
}

bool hasEnding(const String& str, const char* ending)
{
	const size_t endingLength = strlen(ending);
    if (str.length() >= endingLength) {
        return (0 == str.compare (str.length() - endingLength, endingLength, ending));
    } else {
        return false;
    }
}

} // namespace cz

