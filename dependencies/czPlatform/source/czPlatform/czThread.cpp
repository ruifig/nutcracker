/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com

	created:	2012/03/10
	created:	10:3:2012   17:18
	filename: 	czThread.cpp

	purpose:
	Include code equivalent to what you can find in C++1 <thread> header
*********************************************************************/

#include "czPlatformStableHeaders.h"
#include "czThread.h"

// Include platform specific code
#if CZ_PLATFORM == CZ_PLATFORM_WIN32
	#include "detail/czThread_win32_cpp.h"
#elif CZ_PLATFORM == CZ_PLATFORM_SYMBIAN
	#include "detail/czThread_pthreads_cpp.h"
#endif


namespace cz
{

//////////////////////////////////////////////////////////////////////////
// C++11 compatible(ish) thread
//////////////////////////////////////////////////////////////////////////

void thread::start_thread()
{
	// Increment reference count, so the object doesn't get destroyed before the new thread starts.
	// This is a way to pass the "reference" to the code in the new thread context. The code that
	// calls the user callable is responsible for releasing this reference
	intrusive_ptr_add_ref(thread_info.get());

	// Call platform implementation
	if (!detail::thread_implementation::_start(thread_info.get()) )
	{
		// thread failed to start, so manually remove the reference count we had reserved for it
		intrusive_ptr_release(thread_info.get());
	}

}

void thread::release_handle()
{
	thread_info = 0;
}

void thread::swap(thread& other)
{
	thread_info.swap(other.thread_info);
}

bool thread::joinable() const
{
	return (get_thread_info)();
}

void thread::join()
{
	detail::thread_data_ptr local_thread_info=(get_thread_info)();
	if(local_thread_info)
	{
		// call platform implementation
		detail::thread_implementation::_join(local_thread_info.get());

		// Thread has finished, so we release the handle
		release_handle();
	}
}

void thread::detach()
{
	release_handle();
}

detail::thread_data_ptr thread::get_thread_info () const
{
	return thread_info;
}

thread::~thread()
{
	detach();
}

} // namespace cz
