/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com	
	purpose:
*********************************************************************/

#ifndef czConditionVariable_pthreads_h__
#define czConditionVariable_pthreads_h__

#include "czPlatformPrerequisites.h"

namespace cz
{
	namespace detail
	{

		class condition_variable_implementation
		{
		private:
			// private and undefined to avoid use
		    condition_variable_implementation(const condition_variable_implementation&);
		    condition_variable_implementation& operator=(const condition_variable_implementation&);
		public:
		    condition_variable_implementation()
			{
			    CZ_CHECK( !pthread_cond_init(&mVar, NULL) );
			}
			
			~condition_variable_implementation()
			{
			    int ret;
			    do
			    {
			        ret = pthread_cond_destroy(&mVar);
			    } while ( ret == EINTR);
			    CZ_CHECK(!ret);
			}
			void notify_one()
			{
				CZ_CHECK( !pthread_cond_signal(&mVar) );
			}
			void notify_all()
			{
                CZ_CHECK( !pthread_cond_broadcast(&mVar) );			    
			}
			void wait(cz::unique_lock<cz::mutex>& lck)
			{
			    int ret;
			    do
			    {
			        ret = pthread_cond_wait(&mVar, lck.mutex()->_getHandle());
			    } while( ret == EINTR );
			    CZ_CHECK ( !ret);
			}
		private:
			pthread_cond_t mVar;
		};
	} // namespace detail
} // namespace cz

#endif // czConditionVariable_win32_h__
