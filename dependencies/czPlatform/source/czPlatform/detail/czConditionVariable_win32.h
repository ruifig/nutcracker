/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com
	
	purpose:
	Windows implementation for condition_variable
*********************************************************************/

#ifndef czConditionVariable_win32_h__
#define czConditionVariable_win32_h__

#include "../czPlatformPrerequisites.h"

namespace cz
{
	namespace detail
	{

#if defined(CZ_WIN32_NATIVE_CONDITION_VARIABLES)
		// Use native implementation: Only available for Windows Vista and up
		class condition_variable_implementation
		{
		private:
			// private and undefined to avoid use
			condition_variable_implementation(const condition_variable_implementation&);
			condition_variable_implementation& operator=(const condition_variable_implementation&);
		public:
			condition_variable_implementation()
			{
				InitializeConditionVariable(&mVar);
			}
			~condition_variable_implementation()
			{
			}
			void notify_one()
			{
				WakeConditionVariable(&mVar);
			}
			void notify_all()
			{
				WakeAllConditionVariable(&mVar);
			}
			void wait(::cz::unique_lock<::cz::mutex>& lck)
			{
				SleepConditionVariableCS(&mVar, lck.mutex()->_getHandle(), INFINITE);
			}
		private:
			CONDITION_VARIABLE mVar;
		};
#else

		// Implementation using mutex/critical section, as explained in http://www.cs.wustl.edu/~schmidt/win32-cv-1.html
		// Implemented variation is "3.4. The SignalObjectAndWait Solution" for everything, except for the wait condition, which is implemented as
		// specified in http://www.cs.wustl.edu/~schmidt/win32-cv-2.html, "4.1. Modifying the SignalObjectAndWait Solution for Windows '95 and Windows NT 3.51"
		//
		class condition_variable_implementation
		{
		private:
			// private and undefined to avoid use
			condition_variable_implementation(const condition_variable_implementation&);
			condition_variable_implementation& operator=(const condition_variable_implementation&);

			typedef struct
			{
				int waiters_count_;
				// Number of waiting threads.

				CRITICAL_SECTION waiters_count_lock_;
				// Serialize access to <waiters_count_>.

				HANDLE sema_;
				// Semaphore used to queue up threads waiting for the condition to
				// become signaled. 

				HANDLE waiters_done_;
				// An auto-reset event used by the broadcast/signal thread to wait
				// for all the waiting thread(s) to wake up and be released from the
				// semaphore. 

				size_t was_broadcast_;
				// Keeps track of whether we were broadcasting or signaling.  This
				// allows us to optimize the code if we're just signaling.
			} cond_t;
			cond_t mData;

		public:

			condition_variable_implementation()
			{
				cond_t* cv = &mData;
				cv->waiters_count_ = 0;
				cv->was_broadcast_ = 0;
				cv->sema_ = CreateSemaphore (NULL,       // no security
											0,          // initially 0
											0x7fffffff, // max count
											NULL);      // unnamed 
				InitializeCriticalSection (&cv->waiters_count_lock_);
				cv->waiters_done_ = CreateEvent (NULL,  // no security
												FALSE, // auto-reset
												FALSE, // non-signaled initially
												NULL); // unnamed
			}

			~condition_variable_implementation()
			{
				CZ_CHECK( CloseHandle(mData.sema_) );
				DeleteCriticalSection(&mData.waiters_count_lock_);
				CZ_CHECK( CloseHandle(mData.waiters_done_) );
			}

			void notify_one()
			{
				cond_t* cv = &mData;
				EnterCriticalSection (&cv->waiters_count_lock_);
				int have_waiters = cv->waiters_count_ > 0;
				LeaveCriticalSection (&cv->waiters_count_lock_);

				// If there aren't any waiters, then this is a no-op.  
				if (have_waiters)
					ReleaseSemaphore (cv->sema_, 1, 0);	
			}

			void notify_all()
			{
				cond_t* cv = &mData;

				// This is needed to ensure that <waiters_count_> and <was_broadcast_> are
				// consistent relative to each other.
				EnterCriticalSection (&cv->waiters_count_lock_);
				int have_waiters = 0;

				if (cv->waiters_count_ > 0) {
					// We are broadcasting, even if there is just one waiter...
					// Record that we are broadcasting, which helps optimize
					// <pthread_cond_wait> for the non-broadcast case.
					cv->was_broadcast_ = 1;
					have_waiters = 1;
				}

				if (have_waiters) {
					// Wake up all the waiters atomically.
					ReleaseSemaphore (cv->sema_, cv->waiters_count_, 0);

					LeaveCriticalSection (&cv->waiters_count_lock_);

					// Wait for all the awakened threads to acquire the counting
					// semaphore. 
					DWORD result = WaitForSingleObject (cv->waiters_done_, INFINITE);
					CZ_CHECK(result==WAIT_OBJECT_0);

					// This assignment is okay, even without the <waiters_count_lock_> held 
					// because no other waiter threads can wake up to access it.
					cv->was_broadcast_ = 0;
				}
				else
					LeaveCriticalSection (&cv->waiters_count_lock_);
			}

			void wait(::cz::unique_lock<::cz::mutex>& lck)
			{
				cond_t* cv = &mData;
				PCRITICAL_SECTION external_mutex = lck.mutex()->_getHandle();

				// Acquire lock to avoid race conditions.
				EnterCriticalSection (&cv->waiters_count_lock_);
				cv->waiters_count_++;
				LeaveCriticalSection (&cv->waiters_count_lock_);

				// The following two function calls are used in place of
				// <SignalObjectAndWait> for versions of Win32 that lack this
				// function.

				// Keep the lock held just long enough to increment the count of
				// waiters by one.  We can't keep it held across the call to
				// <WaitForSingleObject> since that will deadlock other calls to
				// <pthread_cond_signal> and <pthread_cond_broadcast>.
				LeaveCriticalSection (external_mutex);  

				// Wait to be awakened by a <pthread_cond_signal> or
				// <pthread_cond_broadcast>. 
				DWORD result = WaitForSingleObject (cv->sema_, INFINITE);
				CZ_CHECK(result==WAIT_OBJECT_0);

				// Reacquire lock to avoid race conditions.
				EnterCriticalSection (&cv->waiters_count_lock_);

				// We're no longer waiting...
				cv->waiters_count_--;

				// Check to see if we're the last waiter after a
				// <pthread_cond_broadcast> call. 
				int last_waiter = cv->was_broadcast_ && cv->waiters_count_ == 0;

				LeaveCriticalSection (&cv->waiters_count_lock_);

				// If we're the last waiter thread during this particular broadcast
				// then let all the other threads proceed.
				if (last_waiter)
					SetEvent (cv->waiters_done_);

				// Always regain the external mutex since that's the guarantee that
				// we give to our callers.
				EnterCriticalSection (external_mutex);
			}
	
		};



#endif
	}
}

#endif // czConditionVariable_win32_h__
