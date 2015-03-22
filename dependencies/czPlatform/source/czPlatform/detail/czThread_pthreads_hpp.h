/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com
	
	purpose:
	pthreads thread implementation
*********************************************************************/


#ifndef czThread_pthreads_h__
#define czThread_pthreads_h__

#include "czPlatformPrerequisites.h"
#include "czThreadCommon.h"

namespace cz
{
	namespace detail
	{

		struct thread_data_implementation
		{
		    pthread_t thread_handle;

			thread_data_implementation() : thread_handle(0)
			{
			}

			virtual ~thread_data_implementation()
			{
			}

			virtual void run() = 0;
		};

		class thread_implementation
		{
		private:
			// private and undefined to avoid use
			thread_implementation(thread_implementation&);
			thread_implementation& operator=(thread_implementation&);
		public:

			// Called from cz::thread
			// Should return the new thread's id
			static bool _start(detail::thread_data_implementation* threadData);

			// Called from cz::thread
			static void _join(detail::thread_data_implementation* threadData);

			// Called from cz::thread
			static int _getId(detail::thread_data_implementation* threadData)
			{
				return threadData->thread_handle;
			};

			static void _setpriority(detail::thread_data_implementation* threadData, thread_priority priority);

#if CZ_PLATFORM == CZ_PLATFORM_SYMBIAN
			inline static void _yield()
			{
			    User::AfterHighRes(1);
			}

			inline static void _sleep_ms(int milliseconds)
			{
				User::AfterHighRes(milliseconds*1000);
			}
#else
    #error Not implemented
#endif
		};

	}
}

#endif // czThread_pthreads_h__
