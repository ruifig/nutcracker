/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com

	created:	2012/03/10
	created:	10:3:2012   1:12
	filename: 	czAtomicCount_win32.h

	purpose:
	atomic_count used for reference counting. Implementation for win32
*********************************************************************/


#ifndef czAtomicCount_win32_h__
#define czAtomicCount_win32_h__

#include "../czPlatformPrerequisites.h"

namespace cz
{

	class atomic_count
	{
	public:
		explicit atomic_count(long v) : value(v)
		{
		}

		void inc()
		{
#if defined( __MINGW32__ )
			InterlockedIncrement(&value);
#else
			InterlockedIncrement(&value);
#endif
		}

		bool dec_and_test()
		{
#if defined( __MINGW32__ )
			return (InterlockedDecrement(&value)==0) ;
#else
			return (InterlockedDecrement(&value)==0);
#endif
		}

		operator int() const
		{
			return static_cast<long const volatile &> (value);
		}

	private:

		// Private and not defined, so we can't use it
		atomic_count( atomic_count const&);
		atomic_count& operator=( atomic_count const&);

		long value;
	};

}

#endif // czAtomicCount_win32_h__

