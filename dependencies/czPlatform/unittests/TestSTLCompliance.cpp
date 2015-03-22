/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com

	created:	2012/03/05
	created:	5:3:2012   1:07
	filename: 	TestSTLCompliance.cpp

	purpose:
	Miscellaneous tests for STL compliance, where applicable, like type_traits, etc
*********************************************************************/

#include "UnitTest++.h"
#include "czPlatform/czPlatformStableHeaders.h"
#include <functional>
#include <iostream>

// If the compiler has support for std::type_traits, etc, then use my own results against std,
// to check if they are the same
#if _MSC_VER >= 1600
	#define HAS_TR1 1
#endif

void LogError(const char* str, int line)
{
	printf("%s:%d: %s \n", __FILE__, line, str);
}

#define LOG_FAIL(exp) \
	{ \
		LogError(exp, __LINE__); \
		CHECK(0 && "Check previous log for information"); \
	}

#if HAS_TR1
	#include <type_traits>

	#define TR1_CTASSERT(exp, val) \
		CZ_COMPILETIME_ASSERT( cz::exp::value==val && (cz::exp::value == std::exp::value) );

#define TR1_COMPLIANCE_CHECK(exp, val) \
	{ \
		bool r1 = cz::exp::value; \
		bool r2 = std::exp::value; \
		if (r2!=val) \
			LOG_FAIL("Failed: std::"#exp "::value is not "#val " as the test expected"); \
		if (r1!=r2) \
			LOG_FAIL("Failed: cz::"#exp "::value != std::"#exp "::value"); \
	}

#define TR1_CTASSERT(exp, val) \
	CZ_COMPILETIME_ASSERT( cz::exp::value==val && (cz::exp::value == std::exp::value) );

#else
	#define TR1_CTASSERT(exp, val) \
		CZ_COMPILETIME_ASSERT( cz::exp::value==val);

#define TR1_COMPLIANCE_CHECK(exp, val) \
	{ \
		bool r1 = cz::exp::value; \
		if (r1!=val) \
			LOG_FAIL("Failed: cz::"#exp "::value is not "#val " as the test expected"); \
	}

#endif

#define TR1_TEST_TYPE_TRAITS(T, isintegral, isfloating, isarithmetic, isreference, ispointer, isfunction, ismemberfunctionpointer) \
	TR1_COMPLIANCE_CHECK(is_integral<T> , isintegral ); \
	TR1_COMPLIANCE_CHECK(is_floating_point<T> , isfloating ); \
	TR1_COMPLIANCE_CHECK(is_arithmetic<T>, isarithmetic ); \
	TR1_COMPLIANCE_CHECK(is_reference<T> , isreference ); \
	TR1_COMPLIANCE_CHECK(is_pointer<T> , ispointer ); \
	TR1_COMPLIANCE_CHECK(is_function<T> , isfunction ); \
	TR1_COMPLIANCE_CHECK(is_member_function_pointer<T> , ismemberfunctionpointer);

#define TR1_TEST_TYPE_TRAIT_1(trait, val, T1) \
	TR1_COMPLIANCE_CHECK(trait<T1>, val)

#define TR1_TEST_TYPE_TRAIT_2(trait, val, T1, T2) \
	TR1_TEST_TYPE_TRAIT_1(trait, val, T1) \
	TR1_COMPLIANCE_CHECK(trait<T2>, val)

#define TR1_TEST_TYPE_TRAIT_3(trait, val, T1, T2, T3) \
	TR1_TEST_TYPE_TRAIT_2(trait, val, T1, T2) \
	TR1_COMPLIANCE_CHECK(trait<T3>, val)

#define TR1_TEST_TYPE_TRAIT_4(trait, val, T1, T2, T3, T4) \
	TR1_TEST_TYPE_TRAIT_3(trait, val, T1, T2, T3) \
	TR1_COMPLIANCE_CHECK(trait<T4>, val)

#define TR1_TEST_TYPE_TRAIT_5(trait, val, T1, T2, T3, T4, T5) \
	TR1_TEST_TYPE_TRAIT_4(trait, val, T1, T2, T3, T4) \
	TR1_COMPLIANCE_CHECK(trait<T5>, val)

#define TR1_TEST_TYPE_TRAIT_6(trait, val, T1, T2, T3, T4, T5, T6) \
	TR1_TEST_TYPE_TRAIT_5(trait, val, T1, T2, T3, T4, T5) \
	TR1_COMPLIANCE_CHECK(trait<T6>, val)

#define TR1_TEST_TYPE_TRAIT_7(trait, val, T1, T2, T3, T4, T5, T6, T7) \
	TR1_TEST_TYPE_TRAIT_6(trait, val, T1, T2, T3, T4, T5, T6) \
	TR1_COMPLIANCE_CHECK(trait<T7>, val)

#define TR1_TEST_TYPE_TRAIT_8(trait, val, T1, T2, T3, T4, T5, T6, T7, T8) \
	TR1_TEST_TYPE_TRAIT_7(trait, val, T1, T2, T3, T4, T5, T6, T7) \
	TR1_COMPLIANCE_CHECK(trait<T8>, val)

#define TR1_TEST_TYPE_TRAIT_9(trait, val, T1, T2, T3, T4, T5, T6, T7, T8, T9) \
	TR1_TEST_TYPE_TRAIT_8(trait, val, T1, T2, T3, T4, T5, T6, T7, T8) \
	TR1_COMPLIANCE_CHECK(trait<T9>, val)

#include "czPlatform/czTypeTraits.h"
#include "czPlatform/czTuple.h"
#include "czPlatform/czFunctional.h"
#include "czPlatform/czCallTraits.h"

class OtherClass
{
	int mInt;
};
class SomeClass
{
public:
	void f1();
	int f2() { return 0;}
	int f3(int) { return 0; }
	int f4(int) const { return 0; }
	int f5(int) volatile { return 0; }
	int f6(int) const volatile { return 0; }
	int mFloat;
	int mInt;
	OtherClass mOtherClass;
};

struct CallableClass
{
	int operator() ()
	{
		return 1;
	}
};

namespace
{

	TEST(TestTypeTraits)
	{
		TR1_TEST_TYPE_TRAIT_7(is_arithmetic, true, char, int, const char, const int, float, double, const float);
		TR1_TEST_TYPE_TRAIT_9(is_integral, true , char, wchar_t, unsigned char, short, unsigned short, int, unsigned int, const int, const volatile int);
		TR1_TEST_TYPE_TRAIT_5(is_integral, false, char&, char*, float, double, SomeClass);
		TR1_TEST_TYPE_TRAIT_2(is_floating_point, true, float, double);
		TR1_TEST_TYPE_TRAIT_5(is_reference, true , int&, const int&, const volatile int&, int*&, const int*&);
		TR1_TEST_TYPE_TRAIT_5(is_reference, false, int, const int, const volatile int, int*, const int*);
		TR1_TEST_TYPE_TRAIT_4(is_pointer, true, int*, const int*, void (*)(int), void(*)(int));

		TR1_TEST_TYPE_TRAIT_2(is_pointer, false,
			void (SomeClass::*)(), // A pointer to member function should not be considered a point, according to the standard
			int*& // reference to pointer is not a pointer
			);

		TR1_TEST_TYPE_TRAIT_3(is_function, true,
			void (),
			void (int, int),
			int ());

		// Function pointer should not be considered functions, according to the standard
		TR1_TEST_TYPE_TRAIT_4(is_function, false,
			void (*),
			void (*)(int, int),
			int (*)(),
			CallableClass // callable objects should not be considered functions
			);

		TR1_TEST_TYPE_TRAIT_6(is_member_function_pointer, true,
			void (SomeClass::*)(),
			int (SomeClass::*)(),
			int (SomeClass::*)(int),
			int (SomeClass::*)(int) const,
			int (SomeClass::*)(int) volatile,
			int (SomeClass::*)(int) const volatile
			);
	}

	TEST(TestCallTraits)
	{

		// For a 32 bits build, a int64 should be passed as a reference
#if CZ_ARCH_TYPE == CZ_ARCHITECTURE_32
		TR1_TEST_TYPE_TRAITS(cz::call_traits<cz::u64>::param_type, false, false, false, true, false, false, false);
#else
		// For a 64 bits build, a int64 should be passed as a value
		TR1_TEST_TYPE_TRAITS(cz::call_traits<cz::u64>::param_type, true, false, true, false, false, false, false);
#endif

		TR1_TEST_TYPE_TRAITS(cz::call_traits<int>::param_type, true, false, true, false, false, false, false);
		TR1_TEST_TYPE_TRAITS(cz::call_traits<OtherClass>::param_type, false, false, false, true, false, false, false);
		TR1_TEST_TYPE_TRAITS(cz::call_traits<const OtherClass&>::param_type, false, false, false, true, false, false, false );
		TR1_TEST_TYPE_TRAITS(cz::call_traits<OtherClass*>::param_type, false, false, false, false, true, false, false);
		TR1_TEST_TYPE_TRAITS(cz::call_traits<const OtherClass*>::param_type, false, false, false, false, true, false, false);
	}


	template<typename T>
	struct PrimitiveWrap
	{
		PrimitiveWrap() : v(0) {}
		PrimitiveWrap(T v_) : v(v_) {}
		T v;
		operator T() const { return v;}
	};

	TEST(TestTuple)
	{
		// Test tuple with value, reference and pointer
		{
			int a=1;
			float b=2;
			double c=3;
			cz::tuple<int,float&,double*> t(a,cz::ref(b), &c);
			cz::get<0>(t) = 4;
			cz::get<1>(t) = 5.0f; // should change b
			*cz::get<2>(t) = 6.0f; // should change c
			CHECK(a==1);
			CHECK(b==5);
			CHECK(c==6.0f);

			// Test assignment/conversion between different tuples
			double d=0;
			t = cz::make_tuple(PrimitiveWrap<int>(1), PrimitiveWrap<float>(2), PrimitiveWrap<double*>(&d));
			cz::get<0>(t) = 7;
			cz::get<1>(t) = 8.0f; // should change b
			*cz::get<2>(t) = 9.0f; // should change c
			CHECK(b==8.0);
			CHECK(d==9.0f);
		}

	}

}

