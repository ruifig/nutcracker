/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com
	
	purpose:
	thread code and definitions that both the main thread code and the platform
	dependent code knows about
*********************************************************************/


#ifndef czThreadCommon_h__
#define czThreadCommon_h__

namespace cz
{

	/*! \addtogroup Threading
		@{
	*/

	/*! Thread priority*/
	enum thread_priority
	{
		THREADPRIORITY_LOWEST=0,
		THREADPRIORITY_BELOW_NORMAL,
		THREADPRIORITY_NORMAL,
		THREADPRIORITY_ABOVE_NORMAL,
		THREADPRIORITY_HIGHEST,
		THREADPRIORITY_TIME_CRITICAL,
		THREADPRIORITY_COUNT // Don't use this one. It's just used internally to know how many priorities are available
	};


	/*!
		@}
	*/
}

#endif // czThreadCommon_h__
