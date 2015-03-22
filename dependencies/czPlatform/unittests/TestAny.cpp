#include "UnitTest++.h"
#include "czPlatform/czPlatformStableHeaders.h"
#include "czPlatform/czAny.h"

using namespace std;
using namespace UnitTest;

#if defined(_MSC_VER)
#pragma warning( disable : 4996 )
#endif


namespace {


	TEST( TestAny1 )
	{
		int i;
		float f;
		cz::String s;
		bool b;
		std::vector<uint8_t> blob;

		cz::Any v(true);
		CHECK(v.asBool(b) == true);
		CHECK(v.asInteger(i) == false);
		CHECK(v.asFloat(f) == false);
		CHECK(v.asString(s) == false);
		CHECK(v.asBlob(blob) == false);
	}

	TEST( TestAny2 )
	{
		int i;
		float f;
		cz::String s;
		bool b;
		std::vector<uint8_t> blob;

		cz::Any v;

		v = (bool) false;
		CHECK( v.asBool(b) && b==false);
		v = (bool) true;
		CHECK( v.asBool(b) && b==true);

		v = (int)1;
		CHECK( v.asInteger(i) && i==1);

		v = (float)3.1415f;
		CHECK( v.asFloat(f) );
		CHECK_CLOSE(3.1415f, f, FLT_MIN);

		v = "Hello";
		CHECK( v.asString(s) && s=="Hello");
		v = cz::String("World");
		CHECK( v.asString(s) && s=="World");
		
		cz::String ss("World!");
		v = ss;
		CHECK( v.asString(s) && s=="World!" && ss=="World!");

	}

}
