/********************************************************************
    CrazyGaze (http://www.crazygaze.com)
    Author : Rui Figueira
    Email  : rui@crazygaze.com
    
    purpose:
    pthreads mutex implementations.
    Based on boost mostly
*********************************************************************/


#ifndef czMutex_pthreads_h__
#define czMutex_pthreads_h__

#include "czPlatformPrerequisites.h"

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

            typedef pthread_mutex_t native_handle;

            recursive_mutex_implementation()
            {
                pthread_mutexattr_t attr;
                CZ_CHECK(!pthread_mutexattr_init(&attr));
                CZ_CHECK(!pthread_mutexattr_settype(&attr,PTHREAD_MUTEX_RECURSIVE));
                CZ_CHECK(!pthread_mutex_init(&m,&attr));
                CZ_CHECK(!pthread_mutexattr_destroy(&attr));
            }

            ~recursive_mutex_implementation()
            {
                CZ_CHECK(!pthread_mutex_destroy(&m));
            }

            void lock()
            {
                CZ_CHECK(!pthread_mutex_lock(&m));
            }

            void unlock()
            {
                CZ_CHECK(!pthread_mutex_unlock(&m));
            }

            bool try_lock()
            {
                int const res=pthread_mutex_trylock(&m);
                CZ_ASSERT(!res || res==EBUSY);
                return !res;
            }

            native_handle* _getHandle()
            {
                return &m;
            }
        private:
            native_handle m;
        };

    }
}

#endif // czMutex_pthreads_h__
