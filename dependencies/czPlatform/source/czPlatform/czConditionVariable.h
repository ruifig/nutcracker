/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com

	created:	2012/03/10
	created:	10:3:2012   18:52
	filename: 	czConditionVariable.h
	
	purpose:
	Contains code similar to C++11 <condition_variable>
*********************************************************************/


#ifndef czConditionVariable_h__
#define czConditionVariable_h__

#include "czMutex.h"

#if CZ_PLATFORM == CZ_PLATFORM_WIN32
	#include "detail/czConditionVariable_win32.h"
#elif CZ_HAS_PTHREADS
    #include "detail/czConditionVariable_pthreads.h"
#else
	#error Not implemented
#endif

namespace cz
{

	/*! Similar to C++11's condition_variable. */
	class condition_variable
	{
	private:
		// private and undefined, to avoid use
		condition_variable(const condition_variable&);
		condition_variable& operator=(const condition_variable&);

		detail::condition_variable_implementation mData;
	public:
		condition_variable()
		{
		}
		~condition_variable()
		{
		}

		/*! Wakes 1 thread waiting on the variable. NOTE: The mutex must be locked when calling this. */
		void notify_one()
		{
			mData.notify_one();
		}

		/*! Wakes all threads waiting on the variable. NOTE: The mutex must be locked when calling this. */
		void notify_all()
		{
			mData.notify_all();
		}

		/*! Waits on the variable. NOTE: The mutex must be locked when calling this.
		According to the standard, this function can return due to spurious wakeups (without another thread calling #notify or #notify_all.
		Most of the time you'll want to keep waiting until a particular condition is satisfied. For this, use the other #wait method which allows
		you to specify a condition.
		*/
		void wait(unique_lock<mutex>& lck)
		{
			CZ_ASSERT(lck.owns_lock());
			mData.wait(lck);
		}

		/*! Same thing as #wait, but keeps waiting until "pred" returns true. NOTE: The mutex must be locked when calling this.
		This is preferred to using just #wait, since #wait can return without a call to notify_one/notify_all.
		*/
		template < class predicate >
		void wait(unique_lock<mutex>& lck, predicate pred)
		{
			CZ_ASSERT(lck.owns_lock());
			while (!pred()) wait(lck);
		}

	};
} // namespace cz


#endif // czConditionVariable_h__
