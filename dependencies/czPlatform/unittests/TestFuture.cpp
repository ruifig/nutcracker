#include "UnitTest++.h"
#include "czPlatform/czPlatformStableHeaders.h"

#include "czPlatform/czFuture.h"
#include "czPlatform/czRandom.h"

#include <iostream>
#include <time.h>
#include <queue>
#include <math.h>

using namespace cz;

namespace unittests
{


namespace
{
	int addVals(int a, int b)
	{
		cz::RandomGenerator rng((cz::u32)time( NULL ));
		this_thread::sleep_ms(rng.gen32Inclusive(1,10));
		printf("Calling %s(%d,%d)\n", __FUNCTION__, a,b);
		return a+b;
	}

} // anonymous namespace

TEST(TestFuture)
{
	std::vector< cz::future<int> > f;
	for (int i=0; i<10; i++)
	{
		f.push_back( cz::async(addVals,i,i) );
	}

	for(int i=0; i<static_cast<int>(f.size()); i++)
	{
		int r = f[i].get();
		CHECK(r==i+i);
	}

}

    
} // namespace unittests
