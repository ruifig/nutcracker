/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com

	created:	2012/02/12
	created:	12:2:2012   16:10
	filename: 	czMemory.cpp
	
	purpose:
	
*********************************************************************/

#include "czPlatformStableHeaders.h"

#include "czMemoryTracker.h"

namespace cz
{

	void* CategoryAllocatorBase::_allocateBytes(size_t size, int category, const char* file, int line, const char* func)
	{
		void* p = PlatformRoot::getSingleton().getAppAllocator()->allocate(size, file, line, func);
#if CZ_MEMORY_TRACKER_ENABLED
		MemoryTracker::getSingleton()._recordAlloc(p, size, category, file, line, func);
#endif
		return p;
	}

	void CategoryAllocatorBase::_deallocateBytes(void* ptr)
	{
#if CZ_MEMORY_TRACKER_ENABLED
		MemoryTracker::getSingleton()._recordDealloc(ptr);
#endif

		// Note: This needs to be AFTER the _recordDealloc, otherwise it will break in multithreaded code,
		// since the code could be interrupted between the deallocate and the _recordDealloc, and run an
		// allocate in between in a separate thread, that could allocate the same memory, thus asserting because that same address
		// was already in the MemoryTracker. I actually got this happening consistently.
		PlatformRoot::getSingleton().getAppAllocator()->deallocate(ptr);
	}

	size_t CategoryAllocatorBase::_getMaxAllocationSize()
	{
		return std::numeric_limits<size_t>::max();		
	}

} // namespace cz
