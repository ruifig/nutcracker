/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com

	created:	2012/03/12
	created:	10:3:2012   1:12
	filename: 	czAtomicCount_win32.h

	purpose:
	atomic_count implementation for platforms that support glib
	http://developer.gnome.org/glib/stable/
*********************************************************************/


#ifndef czAtomicCount_glib_h__
#define czAtomicCount_glib_h__

#include "czPlatformPrerequisites.h"

namespace cz
{
	class atomic_count
	{
	public:
		explicit atomic_count(int v)
		{
		    g_atomic_int_set(&value, v);
		}

		void inc()
		{
		    g_atomic_int_inc(&value);
		}

		bool dec_and_test()
		{
		    return g_atomic_int_dec_and_test(&value);
		}

		operator int() const
		{
			return g_atomic_int_get(&value);
		}

	private:

		// Private and not defined, so we can't use it
		atomic_count( atomic_count const&);
		atomic_count& operator=( atomic_count const&);
			int value;
	};

}

#endif // czAtomicCount_win32_h__

