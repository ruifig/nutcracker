#include "UnitTest++.h"
#include "czPlatform/czPlatformStableHeaders.h"
#include <map>
#include <vector>
#include <algorithm>
#include <time.h>
#include "czPlatform/czArray.h"
#include "czPlatform/czRandom.h"
#include "czPlatform/czPlatformRoot.h"

using namespace UnitTest;
using namespace cz;

namespace {

	template<int L, int H>
	bool checkInRangeInclusive(const int &val)
	{
		return val>=L && val<=H;
	}

	template<int L, int H>
	bool checkInRangeExclusive(const int &val)
	{
		return val>=L && val<H;
	}

	TEST (TestRandomRange)
	{
		//
		// Test random range
		//
		cz::RandomGenerator rng3((unsigned) time( NULL));
		cz::TArray<int> ranged;
		const int low = -10;
		const int high = 10;
		size_t inrange=0;

		// Test inclusive generation
		ranged.clear();
		while(ranged.size()!=1000)
		{
			ranged.push_back(rng3.gen32Inclusive(low,high));
		}
		// Check if all generated elements are valid
		inrange = std::count_if(ranged.begin(), ranged.end(), checkInRangeInclusive<low,high>);
		CHECK(inrange==ranged.size());
		// Check if we generated all elements in the wanted
		std::sort(ranged.begin(), ranged.end());
		size_t numUnique = std::unique(ranged.begin(), ranged.end()) - ranged.begin();
		CHECK(numUnique == high-low+1);

		// Test exclusive generation
		ranged.clear();
		while(ranged.size()!=1000)
		{
			ranged.push_back(rng3.gen32(low,high));
		}
		inrange = std::count_if(ranged.begin(), ranged.end(), checkInRangeExclusive<low,high>);
		CHECK(inrange==ranged.size());
		std::sort(ranged.begin(), ranged.end());
		numUnique = std::unique(ranged.begin(), ranged.end()) - ranged.begin();
		CHECK(numUnique == high-low);
	}

	TEST (TestDoubleRange)
	{
		// Generate doubles from two generators with the same seed
		// Elements generated with the inclusive range must be > to the elements generated with exclusive range,
		cz::RandomGenerator rng1(1234);
		cz::RandomGenerator rng2(1234);
		int count=10;
		while(count--)
		{
			double d1 = rng1.gen(0.0, 1.0); // exclusive range
			double d2 = rng2.genInclusive(0.0, 1.0); // inclusive range
			CHECK(d2>d1);
		}
	}

	TEST (TestRandom)
	{
	    cz::TArray<int> nums1;
	    cz::TArray<int> nums2;
		cz::RandomGenerator rng1(1234);
		cz::RandomGenerator rng2(1234);
		while(nums1.size()!=10000)
		{
			int n = rng1.gen32();
			CHECK(std::find(nums1.begin(), nums1.end(), n)==nums1.end());
			nums1.push_back(n);
			nums2.push_back(rng2.gen32());
		}

		CHECK_ARRAY_EQUAL(nums1, nums2, nums1.size());

	}

}
