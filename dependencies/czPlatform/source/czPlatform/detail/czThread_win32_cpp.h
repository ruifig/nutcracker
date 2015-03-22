/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com

	purpose:
	win32 thread implementation
*********************************************************************/

#include "czPlatformStableHeaders.h"
#include "czThread_win32_hpp.h"

namespace cz
{
	namespace detail
	{
		unsigned __stdcall thread_start_function(void *param)
		{
			detail::thread_data_implementation* threadData = reinterpret_cast<detail::thread_data_implementation*>(param);
			threadData->run();
			return 0;
		}

		bool thread_implementation::_start(detail::thread_data_implementation* threadData)
		{
			threadData->thread_handle = (HANDLE) _beginthreadex(NULL, 0, thread_start_function, threadData, CREATE_SUSPENDED,  &threadData->id);
			CZ_ASSERT(threadData->thread_handle!=0);
			if (threadData->thread_handle==0)
				return false;
			ResumeThread(threadData->thread_handle);
			return true;
		}

		void thread_implementation::_join(detail::thread_data_implementation* threadData)
		{
			CZ_ASSERT(threadData->id!=0);
			DWORD ret = WaitForSingleObject(threadData->thread_handle, INFINITE);
			CZ_ASSERT_F(ret==WAIT_OBJECT_0,  "Call to Thread::join for thread %u failed (result=%u)\n", static_cast<unsigned int>(threadData->id), static_cast<unsigned int>(ret));
		}

		void thread_implementation::_setpriority(detail::thread_data_implementation* threadData, thread_priority priority)
		{
			CZ_ASSERT(threadData && threadData->thread_handle!=0);

			int ospriorities[THREADPRIORITY_TIME_CRITICAL+1] = {
				THREAD_PRIORITY_LOWEST,
				THREAD_PRIORITY_BELOW_NORMAL,
				THREAD_PRIORITY_NORMAL,
				THREAD_PRIORITY_ABOVE_NORMAL,
				THREAD_PRIORITY_HIGHEST,
				THREAD_PRIORITY_TIME_CRITICAL};

				CZ_CHECK(SetThreadPriority(threadData->thread_handle, ospriorities[priority])==TRUE);
		}

		void thread_implementation::_setaffinity(detail::thread_data_implementation* threadData, u32 mask)
		{
			CZ_CHECK(SetThreadAffinityMask(threadData->thread_handle, mask)!=0);
		}



	} // namespace detail
} // namespace cz
