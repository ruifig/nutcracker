/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com

	created:	2012/03/03
	created:	3:3:2012   12:43
	filename: 	czCallTraits.h
	
	purpose:
	Based on boost call_traits
*********************************************************************/

#ifndef czCallTraits_h__
#define czCallTraits_h__

#include "czPlatformPrerequisites.h"

namespace cz
{
	namespace detail
	{
		// helper for call_traits, to evaluate how to pass a parameter to a function
		template <typename T, bool IsSmall>
		struct ct_imp2 // If it's not small, then pass as a const reference
		{
			typedef const T& param_type;
		};

		template <typename T>
		struct ct_imp2<T, true> // If it's small, then pass by value
		{
			typedef const T param_type;
		};

		// helper for call_traits, to evaluate how to pass a parameter to a function, depending if
		// it's a pointer, or the object size
		template <typename T, bool IsPointer, bool SizeCheck>
		struct ct_imp
		{
			typedef const T& param_type;
		};

		// Specialize to pass reference or value depending on the size
		// NOTE: The reason why I'm not basing it directly on the result of is_arithmetic<T>, it's because
		// even if a type is "arithmetic", passing it by value might not be as efficient as passing by reference.
		// For example, a "long long" it's 64bits, and on a 32bits build, it's more  efficient to pass it as a reference
		template<typename T, bool IsPointer>
		struct ct_imp<T, IsPointer, true>
		{
			typedef typename ct_imp2<T, sizeof(T) <= sizeof(void*)>::param_type param_type;
		};

		// For pointers, always pass it by value (pass the pointer itself)
		template <typename T, bool SizeCheck>
		struct ct_imp<T, true, SizeCheck>
		{
			typedef const T param_type;
		};

	}

	template <typename T>
	struct call_traits
	{
		typedef T value_type;
		typedef T& reference;
		typedef const T& const_reference;

		// If T is a pointer or not a pointer but still a small object (int, float, etc), then pass by value
		// if not, pass by reference
		typedef typename detail::ct_imp<
			T,
			is_pointer<T>::value,
			is_arithmetic<T>::value
			>::param_type param_type;
	};

	// Specialization for when T is a reference
	template <typename T>
	struct call_traits<T&>
	{
		typedef T value_type;
		typedef T& reference;
		typedef const T& const_reference;
		typedef T& param_type;
	};

} // namespace cz


#endif // czCallTraits_h__
