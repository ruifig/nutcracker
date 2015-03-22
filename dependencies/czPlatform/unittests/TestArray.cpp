#include "UnitTest++.h"
#include "czPlatform/czPlatformStableHeaders.h"
#include <algorithm>
#include <functional>
#include <vector>
#include <iostream>
#include "czPlatform/czArray.h"

using namespace std;
using namespace UnitTest;

#if defined(_MSC_VER)
#pragma warning( disable : 4996 )
#endif

class SomeClass
{
	int a;
};

namespace cz
{
	typedef std::string String;
}

class Obj
{
public:
	Obj() : mVal(0) {}
	Obj(int val) : mVal(val)
	{
		counter++;
	}

	Obj(const Obj& other)
	{
		counter++;
		mVal = other.mVal;
	}

	Obj& operator=(const Obj& other)
	{
		mVal = other.mVal;
		return *this;
	}

	~Obj()
	{
		counter--;
	}

	static int counter;
	int mVal;
};
int Obj::counter=0;
namespace {

	TEST (TestArrayGrow)
	{
		cz::TArray<double> a(0);
		CHECK(a.capacity()==0);
		a.reserve(1);
		CHECK(a.capacity()==1);
		a.reserve(1);
		CHECK(a.capacity()==1);
		a.reserve(2);
		CHECK(a.capacity()==2);
	}

	TEST (TestArrayPushPop)
	{
		cz::TArray<int> a(1);

		for(int i=0; i<10; i++)
			a.push(i);

		CHECK(a.size()==10);

		for (int i=9; i>=0; i--)
		{
			int v=-1;
			CHECK(a.pop(v) && v==i);
		}

		CHECK(a.size()==0);
	}

	TEST (TestArrayInsertRemove)
	{
		cz::TArray<int> a;
		a.push(2);
		a.push(3);
		a.push(4);

		a.insertAtIndex(0,0);
		a.insertAtIndex(1,1);
		CHECK(a.insertAtIndex(a.size()+1, 5)==false);
		CHECK(a.insertAtIndex(a.size(),5));

		for (int i=0; i<a.size(); i++)
			CHECK(a[i]==i);

	}

	TEST (TestArrayCopy)
	{
		cz::TArray<int> a;
		for(int i=0; i<10; i++)
			a.push(i);

		cz::TArray<int> b;
		b = a;
		for (int i=9; i>=0; i--)
		{
			int v=-1;
			CHECK(b.pop(v) && v==i);
		}
	}

	TEST (TestObjectArray)
	{
		cz::TArray<Obj> a(1);
		CHECK(Obj::counter==0);

		a.push(Obj(0));
		a.push(Obj(2));
		CHECK(Obj::counter==2);
		a.insertAtIndex(1, Obj(1));
		CHECK(Obj::counter==3);

		for(int i=0; i<a.size(); i++)
			CHECK(a[i].mVal==i);

		while(a.pop()) {};

		CHECK(Obj::counter==0);
	}


	TEST (TestArrayOfStrings)
	{
		cz::TArray<cz::String> a(1);
		a.push(cz::String("Hello"));
		a.push(cz::String("World"));
		CHECK(a[0]=="Hello" && a[1]=="World");
	}



	template<class ArrayType>
	void TestArrayIteratorsEmptyArray(ArrayType &a)
	{
		CHECK(a.size()==0);
		std::sort(a.begin(), a.end());
		CHECK(std::find(a.begin(), a.end(), 1)==a.end());
		CHECK(std::remove(a.begin(), a.end(), 1)==a.end());
	}

	template<class ArrayType>
	void TestArrayIterators(ArrayType &a)
	{
		CHECK(a.size()==3);
		CHECK(a.capacity()>=3);

		int a0[3] = {3,1,2};

		std::swap_ranges(a.begin(), a.end(), &a0[0]);

		int a1[3] = {1,2,3};
		CHECK(std::equal(a.begin(), a.end(), &a1[0])==false);
		std::sort(a.begin(), a.end());
		CHECK(std::equal(a.begin(), a.end(), &a1[0])==true);

		int a2[3] = {3,2,1};
		std::sort(a.begin(), a.end(), std::greater<int>());
		CHECK(std::equal(a.begin(), a.end(), &a2[0])==true);

		CHECK(std::find(a.begin(), a.end(), 0)==a.end()); // element not found
		CHECK(std::find(a.begin(), a.end(), 3)==a.begin()); // element found

		CHECK(a.size()==3);

		// Remove the '2'.
		// doing the &(*result), so it works with both TArray and std::vector
		int *newEndPtr = &(*(std::remove(a.begin(), a.end(), 2)));
		CHECK(newEndPtr==&a[2]);

		// With the '2' removed, a copy of '1' should have moved to that position
		CHECK(a[0]==3 && a[1]==1 && a[2]==1);
	}

	TEST (TestArray_Static)
	{
		cz::TStaticArray<int,3,false> a;
		TestArrayIterators(a);
	}

	TEST (TestArray_StaticTrackedSize)
	{
		cz::TStaticArray<int,3,true> a;
		TestArrayIteratorsEmptyArray(a);
		a.push(0); a.push(0); a.push(0);
		TestArrayIterators(a);
	}

	TEST (TestArray_Dynamic)
	{
		cz::TArray<int> a;
		TestArrayIteratorsEmptyArray(a);
		a.push(0); a.push(0); a.push(0);
		TestArrayIterators(a);
	}

	TEST(TestArray_STL_Vector)
	{
		std::vector<int> a;
		TestArrayIteratorsEmptyArray(a);
		a.push_back(0); a.push_back(0); a.push_back(0);
		TestArrayIterators(a);
	}
}
