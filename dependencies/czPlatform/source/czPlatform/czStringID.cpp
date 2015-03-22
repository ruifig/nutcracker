/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com
	
	purpose:
	
*********************************************************************/

#include "czPlatformStableHeaders.h"
#include "czStringID.h"
#include "czFNVHash.h"

namespace cz
{

void StringID::_initialize()
{
	CZ_ASSERT(msEntries.size()==0);

	#define CZ_REGISTER_STRINGID(name) StringID::create(#name);
	#include "czStringIDs.h"

	CZ_ASSERT(msEntries.size()>0);
}

void StringID::_destroy()
{
	// Empty and shrink the array, so it release any memory
	msEntries.clear();
	msEntries.shrink_to_fit();
}

StringID StringID::create(const char* str)
{
	if (msEntries.capacity()==0)
		msEntries.reserve(128);

	msEntries.push_back(Entry());
	msEntries.last().mString = str;
	msEntries.last().mID = FNVHash32((unsigned char*)(const_cast<char*>(msEntries.last().mString.c_str())), static_cast<unsigned int>(msEntries.last().mString.length())).val;

	StringID id;
	id.mIndex = msEntries.size()-1;
	return id;
}

TArray<StringID::Entry> StringID::msEntries(0);

} // namespace cz
