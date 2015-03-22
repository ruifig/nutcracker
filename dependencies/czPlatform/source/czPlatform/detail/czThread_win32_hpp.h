/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com
	
	purpose:
	win32 thread implementation
*********************************************************************/


#ifndef czThread_win32_h__
#define czThread_win32_h__

#include "czPlatformPrerequisites.h"
#include "czThreadCommon.h"

namespace cz
{
	namespace detail
	{

		struct thread_data_implementation
		{
			HANDLE thread_handle;
			unsigned int id;

			thread_data_implementation() : thread_handle(0), id(0)
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
				return threadData->id;
			};

			static void _setpriority(detail::thread_data_implementation* threadData, thread_priority priority);
			static void _setaffinity(detail::thread_data_implementation* threadData, u32 mask);

			inline static void _yield()
			{
				Sleep(0);
			}

			inline static void _sleep_ms(int milliseconds)
			{
				Sleep(milliseconds);
			}
		};

	}
}

#endif // czThread_win32_h__
