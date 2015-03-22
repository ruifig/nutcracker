/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com

	created:	2012/03/10
	created:	10:3:2012   17:00
	filename: 	czMutex_win32.h
	
	purpose:
	Win32 mutex implementations
*********************************************************************/

#ifndef czMutex_win32_h__
#define czMutex_win32_h__

namespace cz
{
	namespace detail
	{
		class recursive_mutex_implementation
		{
		private:
			// private and undefined to avoid use
			recursive_mutex_implementation(const recursive_mutex_implementation&);
			recursive_mutex_implementation& operator=(const recursive_mutex_implementation&);
		public:

			typedef CRITICAL_SECTION native_handle;

			recursive_mutex_implementation()
			{
				InitializeCriticalSection(&mCriticalSection);
			}

			~recursive_mutex_implementation()
			{
				DeleteCriticalSection(&mCriticalSection);
			}

			void lock()
			{
				EnterCriticalSection(&mCriticalSection);
			}

			void unlock()
			{
				LeaveCriticalSection(&mCriticalSection);
			}

			bool try_lock()
			{
				BOOL r = TryEnterCriticalSection(&mCriticalSection);
				return (r==FALSE) ? false : true;
			}

			native_handle* _getHandle()
			{
				return &mCriticalSection;
			}
		private:
			native_handle mCriticalSection;
		};

	}
}

#endif // czMutex_win32_h__
