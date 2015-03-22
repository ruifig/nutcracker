/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com
	
	purpose:
*********************************************************************/

#include "czPlatformStableHeaders.h"
#include "czAsyncCommandQueue.h"
#include "czPlatform/czLogger.h"

namespace cz
{

void AsyncCommandQueue::send(std::function<void()>&& f)
{
	m_queue.push(f);
}

void AsyncCommandQueue::tickImpl(bool wait)
{
	// Instead of having a possible infinite loop (if we get commands faster than we can process them),
	// I'm checking how many items we have at the start then only consume those.
	int todo = m_queue.size();

	// If nothing to do, we wait until there is something
	if (todo==0 && wait)
	{
		std::function<void()> f;
		//czDebug(ID_Log, "AsyncCommandQueue:%p:Going to sleep\n", this);
		m_queue.wait_and_pop(f);
		//czDebug(ID_Log, "AsyncCommandQueue:%p:Waking up\n", this);
		f();
		todo = m_queue.size();
	}

	while (todo--)
	{
		std::function<void()> f;
		bool res = m_queue.try_and_pop(f);
		CZ_ASSERT(res);
		f();
	}
}

//////////////////////////////////////////////////////////////////////////
//	AsyncCommandQueueExplicit
//////////////////////////////////////////////////////////////////////////
void AsyncCommandQueueExplicit::tick(bool wait)
{
	tickImpl(wait);
}


//////////////////////////////////////////////////////////////////////////
//	AsyncCommandQueueAutomatic
//////////////////////////////////////////////////////////////////////////
AsyncCommandQueueAutomatic::AsyncCommandQueueAutomatic()
{
	m_thread = std::thread(&AsyncCommandQueueAutomatic::run, this);
}

AsyncCommandQueueAutomatic::~AsyncCommandQueueAutomatic()
{
	m_queue.push( [this]() { m_finish = true; }) ;
	m_thread.join();
}

void AsyncCommandQueueAutomatic::run()
{
	while (!m_finish)
	{
		tickImpl(true);
	}
}

} // namespace cz

