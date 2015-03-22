#include "UnitTest++.h"
#include "czPlatform/czPlatformStableHeaders.h"
#include "czPlatform/czMemoryTracker.h"
#include "czPlatform/czCallStackWalker.h"

using namespace UnitTest;
using namespace cz;

namespace {

#if CZ_MEMORY_TRACKER_ENABLED
	TEST (TestMemoryTracker)
	{
		/*
		CallStack::init();
		CallStack callstk;
		CallStack::capture(callstk);
		callstk.output(0);

		MemoryTracker::getSingleton().enableCallStackTracing(MemCat_General, true);
		void* ptr1 = CZ_ALLOC(10, MemCat_General);
		void* ptr2 = CZ_ALLOC(20, MemCat_String);
		MemoryTracker::getSingleton().logDetailedMemUsage("czMemoryTracker.log", "Memory Leaks", true);
		CallStack::term();
		*/
	}

#endif
}
