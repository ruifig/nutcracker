/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com

	created:	2012/02/27
	created:	27:2:2012   14:58
	filename: 	czString.h
	
	purpose:	String related classes and code
*********************************************************************/

#ifndef czString_h__
#define czString_h__

#include "czPlatformPrerequisites.h"
#include "czArray.h"
#include "czUTF8String.h"
#include <vector>
#include <locale>

namespace cz
{
#ifdef __clcpp_parse__
	class String
	{
		int dummy;
	};
#else
	typedef std::basic_string<char, std::char_traits<char>, STLAllocator<char, StringCategoryAllocator> > String;
	//typedef std::string String;
#endif

    void stringSplitIntoLines(const char* textbuffer, int buffersize, std::vector<String> *lines);
	void splitString(const char* str, char delimiter, TArray<String> &tokens);
    bool notSpace(int a);

	/*! Checks if a string ends with another substring */
	bool hasEnding(const String& str, const String& ending);
	bool hasEnding(const std::string& str, const std::string& ending);
	bool hasEnding(const String& str, const char* ending);

	// trim from start
    template<class StringType>
	static inline StringType ltrim(const StringType &s_) {
		StringType s = s_;
		s.erase(s.begin(), std::find_if(s.begin(), s.end(), notSpace));
		return s;
	}

	// trim from end
    template<class StringType>
	static inline StringType rtrim(const StringType &s_) {
		StringType s = s_;
		s.erase(std::find_if(s.rbegin(), s.rend(), notSpace).base(), s.end());
		return s;
	}

	// trim from both ends
    template<class StringType>
	static inline StringType trim(const StringType &s) {
		return ltrim(rtrim(s));
	}
	#define CZ_TEMPORARY_STRING_MAX_SIZE 512
	#define CZ_TEMPORARY_STRING_MAX_NESTING 20

	char* getTemporaryString();
	const char* formatString(const char* format, ...);
	char* formatStringVA(const char* format, va_list argptr);


	template<class StringType>
	StringType stringReplaceAll(const StringType& original, const StringType& from, const StringType& to)
	{
		StringType str = original;
		if (from.empty())
			return str;
		size_t start_pos = 0;
		while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
			str.replace(start_pos, from.length(), to);
			start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
		}
		return str;
	}


	//
	// Case insensitive string search copied from http://stackoverflow.com/questions/3152241/case-insensitive-stdstring-find
	//

#ifndef __clcpp_parse__
	// templated version of my_equal so it could work with both char and wchar_t
	template<typename charT>
	struct my_equal {
		my_equal(const std::locale& loc) : loc_(loc) {}
		bool operator()(charT ch1, charT ch2) {
			return std::toupper(ch1, loc_) == std::toupper(ch2, loc_);
		}
	private:
		const std::locale& loc_;
	};

	// find substring (case insensitive)
	template<typename T>
	static int ci_find_substr(const T& str1, const T& str2, const std::locale& loc = std::locale())
	{
		typename T::const_iterator it = std::search(str1.begin(), str1.end(),
			str2.begin(), str2.end(), my_equal<typename T::value_type>(loc));
		if (it != str1.end()) return it - str1.begin();
		else return -1; // not found
	}
#endif

}


#endif // czString_h__

