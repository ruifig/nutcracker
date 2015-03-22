#include "UnitTest++.h"
#include "czPlatform/czPlatformStableHeaders.h"
#include "czPlatform/czScopeGuard.h"

using namespace std;
using namespace UnitTest;

#if defined(_MSC_VER)
#pragma warning( disable : 4996 )
#endif


namespace {


	TEST(TestScopeGuard1)
	{
		int v1 = 123;
		{
			SCOPE_EXIT { v1=456;} ;
			CHECK( v1==123 );
		}
		CHECK(v1==456);
	}

	TEST(TestScopeGuard2)
	{
		int v1 = 123;
		{
			auto g1 = cz::scopeGuard([&](){ v1=456;});
			CHECK( v1==123 );
		}
		CHECK(v1==456);
	}

	TEST(TestScopeGuard3)
	{
		int v1 = 123;
		{
			auto g1 = cz::scopeGuard([&](){ v1=456;});
			CHECK( v1==123 );
			g1.dismiss();
		}
		CHECK(v1==123);
	}
}
