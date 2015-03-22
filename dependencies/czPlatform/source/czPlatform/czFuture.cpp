/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com

	purpose:

*********************************************************************/

#include "czPlatformStableHeaders.h"
#include "czFuture.h"

namespace cz
{

//namespace detail
//{

// TODO : Check what's wrong with TSingleton. AsyncThreadManager needs to be in "detail" namespace
CZ_DEFINE_SINGLETON(AsyncThreadManager)

AsyncThreadManager::AsyncThreadManager()
{
	mFinish = false;
	mNumThreads = 1;
}

AsyncThreadManager::~AsyncThreadManager()
{
	{
		lock_guard<recursive_mutex> lck(mMutex);
		// TODO : This is not that good.
		CZ_ASSERT(mPendingWork.size()==0);
		while(mPendingWork.size())
		{
			detail::AsyncWorkItemBase* p = mPendingWork.front();
			CZ_DELETE (p);
			mPendingWork.pop();
		}
	}

	mFinish = true;
	mWorkAvailable.notify_all(); // wake up all threads, so they detect mFinish==true, and exit
	while(mThreads.size())
	{
		thread *t = mThreads.back();
		t->join();
		CZ_DELETE_T (t, MemCat_General);
		mThreads.pop_back();
	}

	// TODO : delete anything allocated
}

int AsyncThreadManager::getPendingWorkCount() const
{
	lock_guard<recursive_mutex> lck(mMutex);
	return static_cast<int>(mPendingWork.size());
}

int AsyncThreadManager::getNumThreads() const
{
	return mNumThreads;
}

void AsyncThreadManager::setNumThreads(int numThreads)
{
	lock_guard<recursive_mutex> lck(mMutex);
	mNumThreads = numThreads;
}

void AsyncThreadManager::createThreads()
{
	lock_guard<recursive_mutex> lck(mMutex);
	while(static_cast<int>(mThreads.size()) < mNumThreads)
	{
		mThreads.push_back(
			CZ_NEW_T(cz::thread, MemCat_General) (itemConsumer, cz::ref(mPendingWork), cz::ref(mWorkAvailable), cz::ref(mMutex), cz::ref(mFinish))
			);
	}
}

void AsyncThreadManager::_addWork(detail::AsyncWorkItemBase* item)
{
	lock_guard<recursive_mutex> lck(mMutex);

	if (static_cast<int>(mThreads.size())<mNumThreads)
		createThreads();

	mPendingWork.push(item);
	mWorkAvailable.notify_one();
}

void AsyncThreadManager::itemConsumer(WorkQueue& workQueue, condition_variable& workAvailable, recursive_mutex& queueMutex, bool &finished)
{
	while(true)
	{
		cz::unique_lock<cz::recursive_mutex> queueLock(queueMutex);
		while(!finished && workQueue.empty())
			workAvailable.wait(queueLock);

		if (finished)
			break;

		detail::AsyncWorkItemBase* workItem = workQueue.front();
		workQueue.pop();
		// unlock so other threads can remove work items while we compute this one
		queueLock.unlock();

		// Compute the result (this will put the value in the "future" object the user is holding)
		workItem->run();
		CZ_DELETE workItem;
	}
}



//} // namespace detail

} // namespace cz
