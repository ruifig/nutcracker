/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com

	created:	2012/02/26
	created:	26:2:2012   1:40
	filename: 	czPlatform.h

	purpose:

*********************************************************************/
/** @file */

#ifndef _CZ_PLATFORM_H_
#define _CZ_PLATFORM_H_


/*
If we're parsing this with clReflect, tag whatever primitives I need
*/
#ifdef __clcpp_parse__
	#include <clcpp/clcpp.h>

	clcpp_reflect_part(std)
	clcpp_reflect_part(std::basic_string)
	clcpp_reflect_part(std::allocator)
	clcpp_reflect_part(std::vector)
	clcpp_reflect_part(std::pair)
	clcpp_reflect_part(std::shared_ptr)
	clcpp_reflect_part(std::weak_ptr)
	clcpp_reflect_part(std::char_traits)

	clcpp_reflect_part(cz)
	clcpp_reflect_part(cz::String)
	clcpp_reflect_part(cz::STLAllocator)
	clcpp_reflect_part(cz::STLAllocatorBase)
	clcpp_reflect_part(cz::CategoryAllocator)
	clcpp_reflect_part(cz::TSingleton)
	clcpp_reflect_part(cz::TCPSocket)
	clcpp_reflect_part(cz::SocketAddress)
	clcpp_reflect_part(cz::TCPSocketListener)
	clcpp_reflect_part(cz::TCPSocket::ClosedReason)
	clcpp_reflect_part(cz::TAllocatedObject)
	clcpp_reflect_part(cz::TBaseArray)
	clcpp_reflect_part(cz::TArray)
	clcpp_reflect_part(cz::TStaticArray)
	clcpp_reflect_part(cz::RingBuffer)
	clcpp_reflect_part(cz::Stream)

#else
	// Not running clReflect, so just declare these as empty
	#define clcpp_reflect(name)
	#define clcpp_reflect_part(name)
	#define clcpp_container_iterators(container, read_iterator, write_iterator, keyinfo)
	#define clcpp_attr(...)
	#define clcpp_push_attr(...)
	#define clcpp_pop_attr(...)
#endif




/*! Root namespace
*/
namespace cz
{
}

// Prepare the configuration
#include "czPlatformConfig.h"

// Disable some warning
#if (CZ_PLATFORM==CZ_PLATFORM_WIN32) && defined(_MSC_VER)
#	pragma warning( disable: 4996) // Disable all the deprecated warning for C standard libraries
#endif

namespace cz
{
	typedef unsigned char u8;
	typedef signed char s8;
	typedef unsigned short u16;
	typedef signed short s16;
	typedef unsigned int u32;
	typedef signed int s32;
	typedef signed long long s64;
	typedef unsigned long long u64;
}

/*
typedef cz::u32 uint32_t;
typedef cz::s32 int32_t;
typedef cz::u64 uint64_t;
typedef cz::s64 int64_t;
typedef cz::s16 int16_t;
typedef cz::u16 uint16_t;
typedef cz::s8 int8_t;
typedef cz::u8 uint8_t;
*/

#define CZ_S8_MIN -128
#define CZ_S8_MAX 127
#define CZ_U8 MAX 255
#define CZ_S16_MIN -32768
#define CZ_S16_MAX 32767
#define CZ_U16 MAX 65535
#define CZ_S32_MIN (-2147483647-1)
#define CZ_S32_MAX 2147483647
#define CZ_U32_MAX 0xffffffff

namespace cz
{
	// Assert types
	void _doAssert(const char* file, int line, const char* fmt, ...);

#if CZ_PLATFORM==CZ_PLATFORM_WIN32
	const char* getLastWin32ErrorMessage(int err=0);
#endif
}

/*! \addtogroup Debugging Debugging and logging

   @{
*/

#if CZ_USER_BUILD
	#define CZ_ASSERT(expression) ((void)0)
	#define CZ_ASSERT_F(expression, fmt, ...) ((void)0)
	#define CZ_CHECK(expression) expression
	#define CZ_UNEXPECTED() ::cz::_doAssert(__FILE__, __LINE__, "Unexpected code path")
#else

/*! Checks if the expression is true/false and causes an assert if it's false.
 @hideinitializer
 Depending on the build configuration, asserts might be enabled for release build too
 */
	#define CZ_ASSERT(expression) if (!(expression)) { ::cz::_doAssert(__FILE__, __LINE__, #expression); }
	//#define CZ_ASSERT(expression) (void(0))

/*! Checks if the expression is true/false and causes an assert if it's false.
@hideinitializer
The difference between this and \link CZ_ASSERT \endlink is that it's suitable to display meaningful messages.
\param expression Expression to check
\param fmt printf style format string
*/
#if defined(__GNUG__) || defined(__GNUC__) || (CZ_PLATFORM == CZ_PLATFORM_SYMBIAN)
	#define CZ_ASSERT_F(expression, fmt, ...) if (!(expression)) { ::cz::_doAssert(__FILE__, __LINE__, fmt, ##__VA_ARGS__); } // By using ##__VA_ARGS__ , it will remove the last comma, if __VA_ARGS__ is empty
#else
	#define CZ_ASSERT_F(expression, fmt, ...) if (!(expression)) { ::cz::_doAssert(__FILE__, __LINE__, fmt, ##__VA_ARGS__); }
	#define CZ_FORCE_ASSERT_F(fmt, ...) if (!(expression)) { ::cz::_doAssert(__FILE__, __LINE__, fmt, ##__VA_ARGS__); }
#endif

/*! Evaluates the expression, and asserts if asserts are enabled.
 @hideinitializer
 Note that even if asserts are disabled, it still evaluates the expression (it's not compiled-out like the standard 'assert' for release builds),
 so can be used to check if for example a function returned a specific value:
 \code
 CZ_CHECK( doSomethingCritical()==true );
 \endcode
 */
	#define CZ_CHECK(expression) if (!(expression)) { ::cz::_doAssert(__FILE__, __LINE__, #expression); }

	#define CZ_UNEXPECTED() ::cz::_doAssert(__FILE__, __LINE__, "Unexpected code path")
#endif


#ifdef __clcpp_parse__
	#define CZ_COMPILETIME_ASSERT(compiletime_constant) std::static_assert(compiletime_constant, #compiletime_constant)
#else

namespace cz
{
	template<bool> struct CTAssert;
	template<> struct CTAssert<true> { };
}

/*! This template allows for compile-time asserts.
* @hideinitializer
*
* Example:
* \code
*    CZ_COMPILETIME_ASSERT(sizeof(SomeClass)<=16);
* \endcode
*
* If the size of the the class "SomeClass" >16, then the code won't compile,
* Failing to compile is the expected behavior of CZ_COMPILETIME_ASSERT in this case, since your code requires "SomeClass" objects to fit in 16 bytes
*
* \internal
* The code fails to compile if the expression inside CZ_COMPILETIME_ASSERT is false,
* because the templated struct CTAssert is only defined for <true>
* \endinternal
*/
#define CZ_COMPILETIME_ASSERT(compiletime_constant) ::cz::CTAssert<compiletime_constant>()



struct ReentrantCheck
{
	ReentrantCheck(int* counter_)
	{
		counter = counter_;
		CZ_ASSERT(*counter==0);
		*counter++;
	}
	~ReentrantCheck()
	{
		counter--;
	}

	int *counter;
};

#define CHECK_REENTRANCE \
	static int reentrance_check=0; \
	ReentrantCheck reentrance_check_obj(&reentrance_check);

#endif

/*!
	@}
*/


#endif
