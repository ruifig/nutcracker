/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com

	created:	2012/03/03
	created:	3:3:2012   3:35
	filename: 	czTypeTraits.h
	
	purpose:
	Based on boost
*********************************************************************/

#ifndef czTypeTraits_h__
#define czTypeTraits_h__

#include "czPlatformPrerequisites.h"

#define _CZ_DEFINE_HELPER_MACROS
#include "czHelperMacros.h"

namespace cz
{

	//////////////////////////////////////////////////////////////////////////
	// remove_const
	//////////////////////////////////////////////////////////////////////////

	//! Removes top level const qualifier from the type
	template<class T>
	struct remove_const
	{
		typedef T type;
	};

	//! Removes top level const qualifier from the type
	template<class T>
	struct remove_const<const T>
	{
		typedef T type;
	};

	//! Removes top level const qualifier from the type
	template<class T>
	struct remove_const<const T[]>
	{
		typedef T type[];
	};

	//! Removes top level const qualifier from the type
	template<class T, unsigned int ArraySize>
	struct remove_const<const T[ArraySize]>
	{
		typedef T type[ArraySize];
	};

	//////////////////////////////////////////////////////////////////////////
	// remove_volatile
	//////////////////////////////////////////////////////////////////////////

	//! Removes top level volatile qualifier
	template<class T>
	struct remove_volatile
	{
		typedef T type;
	};

	//! Removes top level volatile qualifier
	template<class T>
	struct remove_volatile<volatile T>
	{
		typedef T type;
	};

	//! Removes top level volatile qualifier
	template<class T>
	struct remove_volatile<volatile T[]>
	{
		typedef T type[];
	};

	//! Removes top level volatile qualifier
	template<class T, unsigned int ArraySize>
	struct remove_volatile<volatile T[ArraySize]>
	{
		typedef T type[ArraySize];
	};

	//////////////////////////////////////////////////////////////////////////
	// remove_cv
	//////////////////////////////////////////////////////////////////////////

	//! Removes top level cv-qualifiers (const and volatile)
	template<class T>
	struct remove_cv
	{
		typedef typename remove_const<typename remove_volatile<T>::type>::type type;
	};

	//////////////////////////////////////////////////////////////////////////
	// integral_constant
	//////////////////////////////////////////////////////////////////////////

	//! Same as std::integral_const
	template<class T, T val>
	struct integral_constant
	{
		typedef integral_constant<T, val> type;
		typedef T value_type;
		// NOTE: In C++11 could do:
		// static const T value = val;
		// Using an enum does the trick
		enum { value = val };
	};

	typedef integral_constant<bool, true> true_type;
	typedef integral_constant<bool, false> false_type;


	//////////////////////////////////////////////////////////////////////////
	// is_integral
	// is_floating_point
	//////////////////////////////////////////////////////////////////////////

	namespace detail
	{
		// For non integral types (classes, structs, floating point numbers, etc) inherit from false_type
		template<class T> struct _is_integral : false_type {};

		// Inherit from true_type for integral types (bool, char, int, etc, both signed and unsigned)
		template<> struct _is_integral<bool> : true_type {};
		template<> struct _is_integral<char> : true_type {};
		template<> struct _is_integral<unsigned char> : true_type {};
		template<> struct _is_integral<signed char> : true_type {};
		template<> struct _is_integral<short> : true_type {};
		template<> struct _is_integral<unsigned short> : true_type {};
		//template<> struct _is_integral<signed short> : true_type {};
		template<> struct _is_integral<int> : true_type {};
		template<> struct _is_integral<unsigned int> : true_type {};
		//template<> struct _is_integral<signed int> : true_type {};
		template<> struct _is_integral<long> : true_type {};
		template<> struct _is_integral<unsigned long> : true_type {};
		//template<> struct _is_integral<signed long> : true_type {};
		template<> struct _is_integral<long long> : true_type {};
		template<> struct _is_integral<unsigned long long> : true_type {};
		
		// When using Symbian, if I specialize for wchar_t it just thinks it's a duplicated
		// specialization for unsigned short, so it seems we need to have __wchar_t_defined defined
        #if CZ_PLATFORM == CZ_PLATFORM_SYMBIAN
            #ifndef __wchar_t_defined		
                #error __wchar_t_defined not defined
            #endif
        #endif 
        template<> struct _is_integral<wchar_t> : true_type {};

		// For non floating point types, inherit from false
		template<class T> struct _is_floating_point : false_type {};
		// For floating point types, inherit from true
		template<> struct _is_floating_point<float> : true_type {};
		template<> struct _is_floating_point<double> : true_type {};
		template<> struct _is_floating_point<long double> : true_type {};
	}
	//! Same as std:

	//! Same as std::is_integral
	template<class T>
	struct is_integral : detail::_is_integral< typename remove_cv<T>::type >
	{
	};

	//! Same as std::is_floating_point
	template<class T>
	struct is_floating_point : detail::_is_floating_point< typename remove_cv<T>::type >
	{
	};

	//////////////////////////////////////////////////////////////////////////
	// is_arithmetic
	//////////////////////////////////////////////////////////////////////////
	
	namespace detail
	{
		template<bool> struct _bool_base;
		template<> struct _bool_base<false> : false_type {};
		template<> struct _bool_base<true> : true_type {};
	}
	//! Same as std::is_arithmetic
	template<class T>
	struct is_arithmetic : detail::_bool_base < is_integral<T>::value || is_floating_point<T>::value >
	{
	};

	//////////////////////////////////////////////////////////////////////////
	// is_member_function_pointer and 
	//////////////////////////////////////////////////////////////////////////
	namespace detail
	{
		//
		// helper for is_function
		template<typename T>
		struct _is_function_pointer : false_type {};

		template<typename R>
		struct _is_function_pointer< R (*)() > : true_type {};
		template<typename R, CZ_REP_0_0(class A) >
		struct _is_function_pointer< R (*)( CZ_REP_0_0(A) ) > : true_type {};
		template<typename R, CZ_REP_0_1(class A) >
		struct _is_function_pointer< R (*)( CZ_REP_0_1(A) ) > : true_type {};
		template<typename R, CZ_REP_0_2(class A) >
		struct _is_function_pointer< R (*)( CZ_REP_0_2(A) ) > : true_type {};
		template<typename R, CZ_REP_0_3(class A) >
		struct _is_function_pointer< R (*)( CZ_REP_0_3(A) ) > : true_type {};
		template<typename R, CZ_REP_0_4(class A) >
		struct _is_function_pointer< R (*)( CZ_REP_0_4(A) ) > : true_type {};
		template<typename R, CZ_REP_0_5(class A) >
		struct _is_function_pointer< R (*)( CZ_REP_0_5(A) ) > : true_type {};
		template<typename R, CZ_REP_0_6(class A) >
		struct _is_function_pointer< R (*)( CZ_REP_0_6(A) ) > : true_type {};
		template<typename R, CZ_REP_0_7(class A) >
		struct _is_function_pointer< R (*)( CZ_REP_0_7(A) ) > : true_type {};
		template<typename R, CZ_REP_0_8(class A) >
		struct _is_function_pointer< R (*)( CZ_REP_0_8(A) ) > : true_type {};
		template<typename R, CZ_REP_0_9(class A) >
		struct _is_function_pointer< R (*)( CZ_REP_0_9(A) ) > : true_type {};
		
		//
		// helper for is_member_function_pointer
		template<typename T>
		struct _is_member_function_pointer : false_type {};

		template<typename R,  class C>
		struct _is_member_function_pointer< R (C::*)() > : true_type {};
		template<typename R,  class C, CZ_REP_0_0(class A) >
		struct _is_member_function_pointer< R (C::*)( CZ_REP_0_0(A) ) > : true_type {};
		template<typename R,  class C, CZ_REP_0_1(class A) >
		struct _is_member_function_pointer< R (C::*)( CZ_REP_0_1(A) ) > : true_type {};
		template<typename R,  class C, CZ_REP_0_2(class A) >
		struct _is_member_function_pointer< R (C::*)( CZ_REP_0_2(A) ) > : true_type {};
		template<typename R,  class C, CZ_REP_0_3(class A) >
		struct _is_member_function_pointer< R (C::*)( CZ_REP_0_3(A) ) > : true_type {};
		template<typename R,  class C, CZ_REP_0_4(class A) >
		struct _is_member_function_pointer< R (C::*)( CZ_REP_0_4(A) ) > : true_type {};
		template<typename R,  class C, CZ_REP_0_5(class A) >
		struct _is_member_function_pointer< R (C::*)( CZ_REP_0_5(A) ) > : true_type {};
		template<typename R,  class C, CZ_REP_0_6(class A) >
		struct _is_member_function_pointer< R (C::*)( CZ_REP_0_6(A) ) > : true_type {};
		template<typename R,  class C, CZ_REP_0_7(class A) >
		struct _is_member_function_pointer< R (C::*)( CZ_REP_0_7(A) ) > : true_type {};
		template<typename R,  class C, CZ_REP_0_8(class A) >
		struct _is_member_function_pointer< R (C::*)( CZ_REP_0_8(A) ) > : true_type {};
		template<typename R,  class C, CZ_REP_0_9(class A) >
		struct _is_member_function_pointer< R (C::*)( CZ_REP_0_9(A) ) > : true_type {};

		template<typename R,  class C>
		struct _is_member_function_pointer< R (C::*)() const> : true_type {};
		template<typename R,  class C, CZ_REP_0_0(class A) >
		struct _is_member_function_pointer< R (C::*)( CZ_REP_0_0(A) ) const > : true_type {};
		template<typename R,  class C, CZ_REP_0_1(class A) >
		struct _is_member_function_pointer< R (C::*)( CZ_REP_0_1(A) ) const > : true_type {};
		template<typename R,  class C, CZ_REP_0_2(class A) >
		struct _is_member_function_pointer< R (C::*)( CZ_REP_0_2(A) ) const > : true_type {};
		template<typename R,  class C, CZ_REP_0_3(class A) >
		struct _is_member_function_pointer< R (C::*)( CZ_REP_0_3(A) ) const > : true_type {};
		template<typename R,  class C, CZ_REP_0_4(class A) >
		struct _is_member_function_pointer< R (C::*)( CZ_REP_0_4(A) ) const > : true_type {};
		template<typename R,  class C, CZ_REP_0_5(class A) >
		struct _is_member_function_pointer< R (C::*)( CZ_REP_0_5(A) ) const > : true_type {};
		template<typename R,  class C, CZ_REP_0_6(class A) >
		struct _is_member_function_pointer< R (C::*)( CZ_REP_0_6(A) ) const > : true_type {};
		template<typename R,  class C, CZ_REP_0_7(class A) >
		struct _is_member_function_pointer< R (C::*)( CZ_REP_0_7(A) ) const > : true_type {};
		template<typename R,  class C, CZ_REP_0_8(class A) >
		struct _is_member_function_pointer< R (C::*)( CZ_REP_0_8(A) ) const > : true_type {};
		template<typename R,  class C, CZ_REP_0_9(class A) >
		struct _is_member_function_pointer< R (C::*)( CZ_REP_0_9(A) ) const > : true_type {};

		template<typename R,  class C>
		struct _is_member_function_pointer< R (C::*)() volatile > : true_type {};
		template<typename R,  class C, CZ_REP_0_0(class A) >
		struct _is_member_function_pointer< R (C::*)( CZ_REP_0_0(A) ) volatile > : true_type {};
		template<typename R,  class C, CZ_REP_0_1(class A) >
		struct _is_member_function_pointer< R (C::*)( CZ_REP_0_1(A) ) volatile > : true_type {};
		template<typename R,  class C, CZ_REP_0_2(class A) >
		struct _is_member_function_pointer< R (C::*)( CZ_REP_0_2(A) ) volatile > : true_type {};
		template<typename R,  class C, CZ_REP_0_3(class A) >
		struct _is_member_function_pointer< R (C::*)( CZ_REP_0_3(A) ) volatile > : true_type {};
		template<typename R,  class C, CZ_REP_0_4(class A) >
		struct _is_member_function_pointer< R (C::*)( CZ_REP_0_4(A) ) volatile > : true_type {};
		template<typename R,  class C, CZ_REP_0_5(class A) >
		struct _is_member_function_pointer< R (C::*)( CZ_REP_0_5(A) ) volatile > : true_type {};
		template<typename R,  class C, CZ_REP_0_6(class A) >
		struct _is_member_function_pointer< R (C::*)( CZ_REP_0_6(A) ) volatile > : true_type {};
		template<typename R,  class C, CZ_REP_0_7(class A) >
		struct _is_member_function_pointer< R (C::*)( CZ_REP_0_7(A) ) volatile > : true_type {};
		template<typename R,  class C, CZ_REP_0_8(class A) >
		struct _is_member_function_pointer< R (C::*)( CZ_REP_0_8(A) ) volatile > : true_type {};
		template<typename R,  class C, CZ_REP_0_9(class A) >
		struct _is_member_function_pointer< R (C::*)( CZ_REP_0_9(A) ) volatile > : true_type {};

		template<typename R,  class C>
		struct _is_member_function_pointer< R (C::*)() const volatile > : true_type {};
		template<typename R,  class C, CZ_REP_0_0(class A) >
		struct _is_member_function_pointer< R (C::*)( CZ_REP_0_0(A) ) const volatile > : true_type {};
		template<typename R,  class C, CZ_REP_0_1(class A) >
		struct _is_member_function_pointer< R (C::*)( CZ_REP_0_1(A) ) const volatile > : true_type {};
		template<typename R,  class C, CZ_REP_0_2(class A) >
		struct _is_member_function_pointer< R (C::*)( CZ_REP_0_2(A) ) const volatile > : true_type {};
		template<typename R,  class C, CZ_REP_0_3(class A) >
		struct _is_member_function_pointer< R (C::*)( CZ_REP_0_3(A) ) const volatile > : true_type {};
		template<typename R,  class C, CZ_REP_0_4(class A) >
		struct _is_member_function_pointer< R (C::*)( CZ_REP_0_4(A) ) const volatile > : true_type {};
		template<typename R,  class C, CZ_REP_0_5(class A) >
		struct _is_member_function_pointer< R (C::*)( CZ_REP_0_5(A) ) const volatile > : true_type {};
		template<typename R,  class C, CZ_REP_0_6(class A) >
		struct _is_member_function_pointer< R (C::*)( CZ_REP_0_6(A) ) const volatile > : true_type {};
		template<typename R,  class C, CZ_REP_0_7(class A) >
		struct _is_member_function_pointer< R (C::*)( CZ_REP_0_7(A) ) const volatile > : true_type {};
		template<typename R,  class C, CZ_REP_0_8(class A) >
		struct _is_member_function_pointer< R (C::*)( CZ_REP_0_8(A) ) const volatile > : true_type {};
		template<typename R,  class C, CZ_REP_0_9(class A) >
		struct _is_member_function_pointer< R (C::*)( CZ_REP_0_9(A) ) const volatile > : true_type {};

	}

	//! Same as std::is_function. Checks if T is a function. Note that if T is a pointer to a function, it evaluates to false.
	/*!
	*/
	template<class T>
	struct is_function : detail::_bool_base< detail::_is_function_pointer< typename remove_cv<T>::type *>::value >
	{
	};

	template<class T>
	struct is_function<T&> : false_type { };


	//! Same as std::is_member_function_pointer (tells if T is a pointer to a member function)
	template<class T>
	struct is_member_function_pointer
		: detail::_bool_base< detail::_is_member_function_pointer< typename remove_cv<T>::type >::value >
	{
	};

	//////////////////////////////////////////////////////////////////////////
	// is_pointer
	//////////////////////////////////////////////////////////////////////////
	namespace detail
	{
		template<class T> struct _is_pointer : false_type {};
		template<class T>
		struct _is_pointer<T*> : true_type {};
	}

	//! Same as std::is_pointer
	template<class T>
	struct is_pointer : detail::_is_pointer< typename remove_cv<T>::type >
	{
	};


	//////////////////////////////////////////////////////////////////////////
	// is_reference
	//////////////////////////////////////////////////////////////////////////
	template<class T> struct is_reference : false_type	{ };
	template<class T> struct is_reference<T&> : true_type{ };


	//////////////////////////////////////////////////////////////////////////
	// is_const
	//////////////////////////////////////////////////////////////////////////

	// TODO : Implement is_const

} // namespace cz

#undef _CZ_DEFINE_HELPER_MACROS
#include "czHelperMacros.h"

#endif // czTypeTraits_h__
