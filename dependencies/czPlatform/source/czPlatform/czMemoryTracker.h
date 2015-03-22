/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com

	created:	2012/02/10
	created:	10:2:2012   1:32
	filename: 	C:\Work\crazygaze\czPlatform\src\czMemory.cpp
	file path:	C:\Work\crazygaze\czPlatform\src
	file base:	czMemory
	file ext:	cpp
	
	purpose:
	Detailed memory allocation tracker.
	Memory used by the memory tracker itself comes from the OS, without using any of
	czPlatform allocators, so that it doesn't interfere with the memory stats you're trying to get
	in the first place.
	For this reason, it directly uses STL containers without custom allocators.
	Detailed memory tracking should only be enabled for debug builds
*********************************************************************/

 /** @file */

#ifndef czMemoryTracker_h__
#define czMemoryTracker_h__

#include "czPlatformPrerequisites.h"

#if CZ_MEMORY_TRACKER_ENABLED

#include "czCallStackWalker.h"
#include "czMutex.h"

namespace cz
{

/** \addtogroup Debugging

	@{
*/


/*! Tracks individual memory allocations and deallocations, per memory category.

It keep detailed tracking information of each memory allocation, so it adds considerable overhead to both memory and CPU.
Should only be used for debug builds. In other builds, you can completely remove this class from the code by
setting \link CZ_MEMORY_TRACKER_ENABLED \endlink to 0, or compile it, but disable at run-time with #setEnabled
*/
class MemoryTracker : public TSingleton<MemoryTracker>
{

public:

	MemoryTracker();
	~MemoryTracker();

	/*! Enables or disables memory tracking completely.
	This is useful when you want a build where you can enable/disable memory allocation at run-time,
	instead of completely removing tracking from the build
	*/
	void setEnabled(bool enabled);

	/*! Registers a memory category

	\param categoryId Memory category Id.
	Take a look at \link cz::MemoryCategory \endlink to check how to define your own memory categories.
	\param name Category name, used when logging
	\param parentCategoryId parent category, or -1 if none.
	Organizing categories in trees can make it easier to understand memory usage when logging, 
	if you have lots of	categories
	*/
	void registerCategory(int categoryId, const char* name, int parentCategoryId=-1);

	/*! Enabled/disables call stack tracing per-category, if call stack tracing is available
	*/
	void enableCallStackTracing(int categoryId, bool enabled);

	void _recordAlloc(void* ptr, size_t size, int categoryId, const char* file, size_t line, const char* func);
	void _recordDealloc(void* ptr);

	/*! Returns the allocated memory peak. */
	size_t getPeak() const
	{
		return mPeakMemory;
	}

	/*! Returns the total memory allocated */
	size_t getTotal() const
	{
		return mTotalMemory;
	}

	/*! Logs all the allocated memory
	* \param filename File to log to. If NULL uses the default file
	* \param caption Title to use for this log
	* \param printCallStackTrace If stack tracing is enabled, it will print the stack trace for each allocation record
	*/
	void logDetailedMemUsage(const char* filename, const char* caption, bool printCallStackTrace=false);

	/*! Logs a small summary of allocated memory per category, to the specified file */
	void logOverallMemUsage(const char* filename);

private:
	
	struct Category
	{
		mutable bool printed; // used temporarily to help when logging, when figuring parent categories
		int parentCategoryIndex;
		size_t bytes;
		std::string name;
		bool stackTraceEnabled;
		Category() : parentCategoryIndex(-1), bytes(0), name("UnNamed"), stackTraceEnabled(false)
		{
		}

	};

	std::vector<Category> mCategories;
	mutable std::vector<std::string> mLogBuffer;

	struct Alloc
	{
		size_t id;
		size_t bytes;
		int categoryId; // The categoryId is also the index into the categories vector
	
		// These are private, so I can force the use of getFile, getFunction, etc, so I can log a "NOT AVAILABLE"
		// when those variables are not set
	private:
		size_t line;
		std::string file;
		std::string func;
	public:
#if CZ_CALLSTACKWALKER_ENABLED
		// NOTE : Space for this is allocated/deallocated using malloc/free, so it doesn't get intercepted
		// by custom allocators, possibly causing recursion into MemoryTracker
		CallStack *callstack;
#endif

		Alloc() : id(0), bytes(0), categoryId(0), line(0)
		{
#if CZ_CALLSTACKWALKER_ENABLED
			callstack = NULL;
#endif
		}

		void set(size_t id_, size_t bytes_, int categoryId_, const char* file_, size_t line_,const char* func_)
		{
			id = id_;
			bytes = bytes_;
			categoryId = categoryId_;
			line = line_;
			file = (file_) ? file_ : "";
			func = (func_) ? func_ : "";
		}

		/*
		void copyFrom(const Alloc& other)
		{
			id = other.id;
			bytes = other.bytes;
			categoryId = other.categoryId;
			file = other.file;
			line = other.line;
			func = other.func;
#if CZ_CALLSTACKWALKER_ENABLED
			callstack = (callstack) ? NULL : other.cal
			 (callstack)
			{
				callstack
				callstack->~CallStack();
				free(callstack);
			}
			callstack = other.callstack;
#endif
		}

		explicit Alloc(const Alloc& other) : callstack(NULL)
		{
			copyFrom(other);
		}

		Alloc& operator= (const Alloc& other);
		{
			copyFrom(other);
			return *this;
		}
		*/

		~Alloc()
		{
#if CZ_CALLSTACKWALKER_ENABLED
			if (callstack)
			{
				callstack->~CallStack();
				free(callstack);
			}
#endif
		}

		size_t getLine() const { return line; }
		const char* getFile() const { return (file.size()) ? file.c_str() : "NOT AVAILABLE"; }
		const char* getFunction() const{ return (func.size()) ? func.c_str() : "NOT AVAILABLE"; }
	};

	typedef std::map<void*, Alloc> AllocsMap;
	AllocsMap mAllocs;
	size_t mIDCounter;
	size_t mPeakMemory; // allocated memory peak
	size_t mTotalMemory; // how much memory allocated at the moment
	size_t mPeakNumAllocs; // allocations peak

	Category* getCategory(int categoryId);
	void addToLogBuffer(const char *format, ...) const;
	void logToFile(const char* filename) const;

	recursive_mutex mMutex;

};

/**
	@}
*/

} // namespace cz

#endif

#endif // czMemoryTracker_h__

