/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com

	created:	2012/02/10
	created:	10:2:2012   1:55
	filename: 	C:\Work\crazygaze\czPlatform\src\czMemoryTracker.cpp
	file path:	C:\Work\crazygaze\czPlatform\src
	file base:	czMemoryTracker
	file ext:	cpp
	
	purpose:
*********************************************************************/

#include "czPlatformStableHeaders.h"

#include "czMemoryTracker.h"

#if CZ_MEMORY_TRACKER_ENABLED

namespace cz
{

CZ_DEFINE_SINGLETON(MemoryTracker)

MemoryTracker::MemoryTracker()
{
	mIDCounter = 0;
	mPeakMemory = 0;
	mTotalMemory = 0;
	mPeakNumAllocs = 0;
}

MemoryTracker::~MemoryTracker()
{
}

MemoryTracker::Category* MemoryTracker::getCategory(int categoryId)
{
	// Create categories up to this one if required
	if (categoryId>=static_cast<int>(mCategories.size()))
	{
		mCategories.resize(categoryId+1);
	}

	return &mCategories[categoryId];
}

void MemoryTracker::registerCategory(int categoryId, const char* name, int parentCategoryId)
{
	Category* cat = getCategory(categoryId);
	cat->parentCategoryIndex = parentCategoryId;
	cat->name = name;
}

void MemoryTracker::enableCallStackTracing(int categoryId, bool enabled)
{
	getCategory(categoryId)->stackTraceEnabled = enabled;
}

void MemoryTracker::_recordAlloc(void* ptr, size_t size, int categoryId, const char* file, size_t line, const char* func)
{
	CZ_ASSERT(ptr);

	lock_guard<recursive_mutex> lck(mMutex);

	AllocsMap::iterator it = mAllocs.find(ptr);
	CZ_ASSERT(it==mAllocs.end() && "Allocation with the same address already found in the memory tracker");
	mIDCounter++;


	Alloc& newAlloc = mAllocs[ptr];
	newAlloc.set(mIDCounter, size, categoryId, file, line, func);
#if CZ_CALLSTACKWALKER_ENABLED
	{
		Category* cat = getCategory(categoryId);
		if (cat->stackTraceEnabled)
		{
			newAlloc.callstack = new(malloc(sizeof(CallStack))) CallStack;
			CallStack::capture(*(newAlloc.callstack));
		}
	}
#endif

	static Category* test = NULL;

	// Update category and parent categories
	int catId=categoryId;
	while(catId!=-1)
	{
		Category* cat = getCategory(catId);

		// TODO : If I don't have this here, it seems there is a bug in the compiler that ends up using the wrong
		// register, and I get an access violation in Release build
		/*
		if (!cat)
			break;
			*/

		cat->bytes += size;
		catId = cat->parentCategoryIndex;
	}

	// Update global stats
	mTotalMemory += size;
	if (mAllocs.size() > mPeakNumAllocs)
		mPeakMemory = mAllocs.size();
	if (mTotalMemory > mPeakMemory)
		mPeakMemory = mTotalMemory;
}


void MemoryTracker::_recordDealloc(void* ptr)
{
	if (!ptr)
		return;

	lock_guard<recursive_mutex> lck(mMutex);

	AllocsMap::iterator it = mAllocs.find(ptr);

	if (it==mAllocs.end())
	{
		CZ_ASSERT(0 && "Pointer not found");
		return;
	}

	mTotalMemory -= it->second.bytes;

	// Update category and parent categories
	int catId = it->second.categoryId;

	while(catId!=-1)
	{
		Category* cat = getCategory(catId);
		cat->bytes -= it->second.bytes;
		catId = cat->parentCategoryIndex;
	}


	mAllocs.erase(it);
}


void MemoryTracker::addToLogBuffer(const char *format, ...) const
{
	va_list argptr;
	va_start(argptr, format);

	static char buf[1024*64];
	cz_vsnprintf(buf,sizeof(buf)-1, format, argptr);
	mLogBuffer.push_back(std::string(buf));
}

void MemoryTracker::logToFile(const char* filename) const
{
	FILE* outfile;
	if ((outfile=fopen(filename, "a+"))==NULL) return;

	for (size_t i=0; i<mLogBuffer.size(); i++)
	{
		fprintf(outfile, mLogBuffer[i].c_str() );
		fprintf(outfile, "\n");
	}

	fclose(outfile);

	mLogBuffer.clear();
}

void MemoryTracker::logDetailedMemUsage(const char* filename, const char* caption, bool printCallStackTrace)
{
	lock_guard<recursive_mutex> lck(mMutex);

	mLogBuffer.clear();

	addToLogBuffer("\n--------------------------------");
	addToLogBuffer("MEM LOG : %s", caption);
	addToLogBuffer("        ID |       SIZE | ALLOCATION AT...");

	// sort by size, ignoring the first slot, which is the one for internal use
	// I'm creating a vector with pointers to the alloc information, since its faster to sort (doesn't involve lots of string copies, etc)
	//
	/*
	std::vector<Alloc*> sortedallocs;
	sortedallocs.reserve(m_Allocations.size());
	for (AllocationMap::iterator i = m_Allocations.begin(); i != m_Allocations.end(); ++i)
	{
	sortedallocs.push_back(&i->second);
	}
	// sort by size
	std::sort(sortedallocs.begin(), sortedallocs.end(), Alloc::SortBySize);
	std::vector<Alloc*>::iterator iter = sortedallocs.begin();
	*/

	AllocsMap::iterator iter = mAllocs.begin();

	while(iter!=mAllocs.end())
	{
		const Alloc* alloc = &iter->second;
		Category* cat = getCategory(alloc->categoryId);
		addToLogBuffer("%10d | %10d | %s:%d   (%s)", alloc->id, alloc->bytes, alloc->getFile(), alloc->getLine(), cat->name.c_str());

#if CZ_CALLSTACKWALKER_ENABLED
		if (printCallStackTrace && cat->stackTraceEnabled && alloc->callstack)
		{
			addToLogBuffer(alloc->callstack->createOutput(2,"                 %s, %fs, Line %l\n"));
			addToLogBuffer("");
		}
#endif
		iter++;
	}

	addToLogBuffer("Total current allocations                   : %10d", mAllocs.size());
	addToLogBuffer("Peak of number of allocations               : %10d", mPeakNumAllocs);
	addToLogBuffer("Total bytes allocated                       : %10d", mTotalMemory);
	addToLogBuffer("Peak of allocated bytes                     : %10d", mPeakMemory);
	addToLogBuffer("--------------------------------");

	logToFile(filename);
	logOverallMemUsage(filename);

}

void MemoryTracker::logOverallMemUsage(const char* filename)
{
	lock_guard<recursive_mutex> lck(mMutex);

	mLogBuffer.clear();

	// clear the helper flag
	for (unsigned int pool=0; pool<mCategories.size(); pool++)
		mCategories[pool].printed = false;

	for (unsigned int pool=0; pool<mCategories.size(); pool++)
	{
		addToLogBuffer("%30s : %13d bytes", mCategories[pool].name.c_str(), mCategories[pool].bytes);
	}

	addToLogBuffer("Total = %13d ", getTotal() );
	addToLogBuffer("Peak  = %13d ", getPeak() );
	addToLogBuffer("--------------------------------");
	logToFile(filename);
}


} // namespace cz

#endif
