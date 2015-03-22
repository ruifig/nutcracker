/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com

	created:	2012/03/10
	created:	10:3:2012   17:06
	filename: 	czMutex.h

	purpose:
	Mutexes, similar to what you can find in boost/std <mutex> header
*********************************************************************/


#ifndef czMutex_h__
#define czMutex_h__

#include "czPlatformPrerequisites.h"

#if CZ_PLATFORM == CZ_PLATFORM_WIN32
	#include "detail/czMutex_win32.h"
#elif CZ_HAS_PTHREADS
    #include "detail/czMutex_pthreads.h"
#else
	#error Not implemented
#endif

namespace cz
{

	/*! \addtogroup Threading
		@{
	*/

	/*! Mutex Encapsulates a recursive mutual exclusion object for multithreaded syncronization.
	By recursive, it means that a thread can make multiple calls to lock/unlock.
	*/
	class recursive_mutex
	{
	private:
		// private and undefined, to avoid use
		recursive_mutex(recursive_mutex const&);
		recursive_mutex& operator=(recursive_mutex const&);
		// actual platform-dependent implementation
		detail::recursive_mutex_implementation mData;

	public:

		recursive_mutex() {}
		~recursive_mutex() {}

		/*! Acquires exclusive ownership of the mutex.
		If the mutex is already owned by another thread, the calling thread will block until the mutex is available.
		You can use #try_lock to only acquire if the mutex is available, therefore not blocking if it fails.
		*/
		void lock()
		{
			mData.lock();
		}

		/*! Releases ownership of the mutex.
		 */
		void unlock()
		{
			mData.unlock();
		}

		/*! Tries to acquire ownership of the mutex.
		Unlike #lock, this doesn't block if the mutex can't be acquired.
		\return Returns true the mutex was acquired, false if not (it's owned by another thread)
		*/
		bool try_lock()
		{
			return mData.try_lock();
		}

		detail::recursive_mutex_implementation::native_handle* _getHandle()
		{
			return mData._getHandle();
		}
	};

	/*! Future equivalent to C++11 mutex (non-recursive).
	At the moment it's the same as mutex_recursive. It's just defined so it can be used where normally
	a proper mutex would be used.*/
	typedef recursive_mutex mutex;

	/*! Similar to boost/C++11 lock_guard.
	It's always preferable to wrap a mutex with lock_guard, so it automatically gets unlocked,
	instead of manually calling lock/unlock.
	*/
	template <class Lockable>
	class lock_guard
	{
	private:
		// private and undefined to avoid use
		lock_guard(lock_guard const&);
		lock_guard& operator=(lock_guard&);
	protected:
		Lockable& mMutex;
	public:
		typedef Lockable mutex_type;

		explicit lock_guard(Lockable& m_) : mMutex(m_)
		{
			mMutex.lock();
		}

		~lock_guard()
		{
			mMutex.unlock();
		}
	};

	/*! Future equivalent of C++11 unique_lock.
	At the moment it doesn't implement all the features from the C++11 equivalent.
	*/
	template<class Lockable>
	class unique_lock
	{
	private:
		// private and undefined to avoid use
		unique_lock(unique_lock const&);
		unique_lock& operator=(unique_lock&);
	protected:
		Lockable* mMutex;
		bool mIsLocked;
	public:
		typedef Lockable mutex_type;

		explicit unique_lock(Lockable& m_) : mMutex(&m_)
		{
			mIsLocked = true;
			mMutex->lock();
		}
		~unique_lock()
		{
			if (mIsLocked)
				mMutex->unlock();
		}

		void lock()
		{
			CZ_ASSERT_F(mIsLocked==false, "unique_lock already has ownership of the mutex." );
			CZ_ASSERT_F(mMutex!=0, "unique_lock not associated to a mutex.");
			mIsLocked = true;
			mMutex->lock();
		}

		bool try_lock()
		{
			CZ_ASSERT_F(mIsLocked==false, "unique_lock already has ownership of the mutex.");
			CZ_ASSERT_F(mMutex!=0, "unique_lock not associated to a mutex.");
			return mMutex->try_lock();
		}

		void unlock()
		{
			CZ_ASSERT_F(mIsLocked==true, "unique_lock doesn't have ownership of the mutex.");
			CZ_ASSERT_F(mMutex!=0, "unique_lock not associated to a mutex.");
			mMutex->unlock();
			mIsLocked = false;
		}

		mutex_type* mutex() const
		{
			return mMutex;
		}

		mutex_type* release()
		{
			mutex_type* m = mMutex;
			mMutex = 0;
			mIsLocked = false;
			return m;
		}

		bool owns_lock() const
		{
			return mIsLocked;
		}

		operator bool() const
		{
			return owns_lock();
		}

	};

	/*!
		@}
	*/
} // namespace cz

#endif // czMutex_h__

