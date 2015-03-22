/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com
	
	purpose:
	pthreads thread implementation
*********************************************************************/

#include "czPlatformStableHeaders.h"
#include "czThread_pthreads_hpp.h"

namespace cz
{
	namespace detail
	{
		void* thread_start_function(void *param)
		{
			detail::thread_data_implementation* threadData = reinterpret_cast<detail::thread_data_implementation*>(param);
			threadData->run();
			return 0;
		}

		bool thread_implementation::_start(detail::thread_data_implementation* threadData)
		{
			int const res = pthread_create(
			    &threadData->thread_handle, // where we get the thread id
			    NULL, // no attributes
			    thread_start_function, // function to call
			    threadData  // the parameter we get in the function call
			    );
			CZ_ASSERT(res==0);
			return (res==0) ? true : false;
		}

		void thread_implementation::_join(detail::thread_data_implementation* threadData)
		{
		    CZ_ASSERT(threadData->thread_handle!=0);
			void* result=0; // will  have the thread return status, if it was specified in the target thread's call to pthread_exit
			CZ_CHECK( !pthread_join(threadData->thread_handle, &result));
		}

		void thread_implementation::_setpriority(detail::thread_data_implementation* threadData, thread_priority priority)
		{
			CZ_ASSERT(threadData && threadData->thread_handle!=0);
			int sched_policy=0;
			
			pthread_attr_t attr;
			sched_param param;
			param.sched_priority=0;
			CZ_CHECK( !pthread_attr_init(&attr)); // Get default attributes
			CZ_CHECK( !pthread_attr_getschedpolicy(&attr, &sched_policy)); // Get scheduler policy used
			CZ_CHECK( !pthread_attr_getschedparam(&attr, &param) ); // Get thread default priority
			// get min/max priorites supported by the default policy
			int minpriority = sched_get_priority_min(sched_policy);
            int maxpriority = sched_get_priority_max(sched_policy);
            
            //printf("Policy=%d [%d, %d], default priority=%d\n", sched_policy, minpriority, maxpriority, param.sched_priority);
            
            int ospriorities[THREADPRIORITY_COUNT] =
            {
                minpriority, //THREADPRIORITY_LOWEST
                minpriority + (param.sched_priority-minpriority)/2, // THREADPRIORITY_BELOW_NORMAL
                param.sched_priority, // THREADPRIORITY_NORMAL,
                param.sched_priority + (maxpriority-param.sched_priority)/3, // THREADPRIORITY_ABOVE_NORMAL
                param.sched_priority + (2*(maxpriority-param.sched_priority))/3, // THREADPRIORITY_HIGHEST
                param.sched_priority + (3*(maxpriority-param.sched_priority))/3 // THREADPRIORITY_TIME_CRITICAL
            };
            
            CZ_CHECK( !pthread_attr_destroy(&attr) );
            
            /*
            printf("NativePriorities[%d]={%d", THREADPRIORITY_COUNT, ospriorities[0]);
            for(int i=1; i<THREADPRIORITY_COUNT; i++)
                printf(",%d", ospriorities[i]);
            printf("}\n");
            */

            // set request priority
            param.sched_priority = ospriorities[priority];
            CZ_CHECK( !pthread_attr_setschedparam(&attr, &param) );
            //printf("setting priority set to %d\n", ospriorities[priority]);
            CZ_CHECK( !pthread_setschedparam(threadData->thread_handle, sched_policy, &param));
            //printf("done", ospriorities[priority]);
			    
		}
	} // namespace detail
} // namespace cz
