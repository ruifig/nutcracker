//
// CrazyGaze (http://www.crazygaze.com)
// Author : Rui Figueira
// Email  : rui@crazygaze.com
//
// --------------------------------------------------------------
//
//

#include "UnitTest++.h"
#include "TestReporterStdout.h"
#include "czPlatform/czPlatformStableHeaders.h"
#include "czPlatform/czMemoryTracker.h"


void EnableUTF8Support()
{
#if CZ_PLATFORM == CZ_PLATFORM_WIN32
	BOOL ret = SetConsoleOutputCP(65001); // utf codepage, as in http://msdn.microsoft.com/en-us/library/dd317756(v=vs.85).aspx
	ret = SetConsoleCP(65001);
#endif
}


int CustomRunAllTests()
{
	using namespace UnitTest;
	TestReporterStdout reporter;
	TestRunner runner(reporter);
	int result=0;
	{
		cz::PlatformRoot root;
#if CZ_CALLSTACKWALKER_ENABLED
		cz::CallStack::init();
#endif

#if CZ_MEMORY_TRACKER_ENABLED
		cz::MemoryTracker::getSingleton().enableCallStackTracing(cz::MemCat_General, true);
#endif

		result = runner.RunTestsIf(Test::GetTestList(), NULL, True(), 0);
	}

#if CZ_MEMORY_TRACKER_ENABLED
	if (cz::MemoryTracker::getSingleton().getTotal()!=0)
	{
		cz::MemoryTracker::getSingleton().logDetailedMemUsage("czMemoryTracker.log", "Memory Leaks", true);
		static_cast<TestReporter*>(&reporter)->ReportFailure(TestDetails("Exit", "", __FILE__, __LINE__), "Memory leaks detected.");
		result +=1;
	}
#endif

#if CZ_CALLSTACKWALKER_ENABLED
	cz::CallStack::term();
#endif

	return result;
}


int main(int, char const *[])
{
	using namespace cz;
	EnableUTF8Support();

	int r = CustomRunAllTests();

	// TODO Remove this
	printf("Press a key and ENTER to finish.\n");
	fflush(stdout);
	getchar();
	return r;
}
