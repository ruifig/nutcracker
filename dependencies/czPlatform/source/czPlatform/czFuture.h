/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com

	created:	2012/03/20
	filename: 	czFuture.h

	purpose:
	Contains code similar to C++11 <future>
*********************************************************************/

#ifndef czFuture_h__
#define czFuture_h__

#include "czPlatformPrerequisites.h"
#include "czMutex.h"
#include "czAtomicCount.h"
#include "czThread.h"
#include "czConditionVariable.h"
#include "czFunctional.h"
#include <queue>
#include <vector>

namespace cz
{


	namespace future_status
	{
		enum future_status {
			ready,
			timeout,
			deferred
		};
	}

	namespace detail
	{
		class shared_state_base;
		void intrusive_ptr_add_ref(shared_state_base * p);
		void intrusive_ptr_release(shared_state_base * p);

		class shared_state_base : public TAllocatedObject<GeneralCategoryAllocator>
		{
		public:
			atomic_count count;

			shared_state_base() : count(0)
			{}

			virtual~ shared_state_base()
			{
			}

			friend void intrusive_ptr_add_ref(shared_state_base* p)
			{
				p->count.inc();
			}

			friend void intrusive_ptr_release(shared_state_base* p)
			{
				if (p->count.dec_and_test())
				{
					CZ_DELETE p;
				}
			}

		protected:
			recursive_mutex mMutex;
		};

		template<class R>
		class shared_state : public shared_state_base
		{
		public:
			shared_state() : mStatus(future_status::deferred)
			{
			}
			virtual ~shared_state()
			{
			}

			bool is_ready() const
			{
				return (mStatus==future_status::ready);
			}

			void set_value(const R& val)
			{
				CZ_ASSERT(!is_ready());
				unique_lock<recursive_mutex> lck(mMutex);
				mResult = val;
				mStatus = future_status::ready;
				mCondVar.notify_all();
			}

			// blocks until the result is available
			void wait()
			{
				unique_lock<recursive_mutex> lck(mMutex);
				while(!is_ready()) mCondVar.wait(lck);
			}

			// Gets the value, blocking if the value is not yet available
			R& get()
			{
				if (!is_ready())
					wait();
				return mResult;
			}

		protected:
			condition_variable mCondVar;
			future_status::future_status mStatus;
			R mResult;
		};

	} // namespace detail

	/*! Similar to std::future . */
	template<class R>
	class future
	{
	public:
		future()
		{
		}

		~future()
		{
		}

		/*! Checks if the future refers to a shared state. */
		bool valid() const
		{
			return (mSharedState.get()==NULL) ? false : true;
		}

		/*! Blocks the value is available. */
		void wait() const
		{
			mSharedState->wait();
		}

		/*! Retrieve the value, blocking if the value is not available yet. */
		R& get()
		{
			return mSharedState->get();
		}


		static future _create(intrusive_ptr<detail::shared_state<R> >& sharedState)
		{
			return future(sharedState);
		}

	protected:

		//friend template<class R> class promise;

		/* For internal use only, when calling promise::get_future(). */
		future(intrusive_ptr<detail::shared_state<R> >& sharedState) : mSharedState(sharedState)
		{
		}

		intrusive_ptr<detail::shared_state<R> > mSharedState;
	private:
		// private and undefined to avoid use
		/*
		future(const future& rhs);
		future& operator=(const future& rhs);
		*/
	};

	/*! Similar to std::promise . */
	template<class R>
	class promise
	{
	public:
		explicit promise() : mSharedState(CZ_NEW detail::shared_state<R>)
		{
			// TODO
		}

		~promise()
		{
			// TODO
		}

		/*! Retrieves the future. */
		future<R> get_future()
		{
			return future<R>::_create(mSharedState);
		}

		/*! Sets the result. */
		void set_value(const R& r)
		{
			mSharedState->set_value(r);
		}

	protected:
		intrusive_ptr<detail::shared_state<R> > mSharedState;
	private:
		// Private and undefined to avoid use
		/*
		promise(const promise& rhs);
		promise& operator=(const promise& rhs);
		*/
	};

	namespace detail
	{

		class AsyncWorkItemBase : public TAllocatedObject<GeneralCategoryAllocator>
		{
		public:
			virtual ~AsyncWorkItemBase() {};
			virtual void run() = 0;
		};

		template<class R, class F>
		class AsyncWorkItem : public AsyncWorkItemBase
		{
		public:
			explicit AsyncWorkItem(const promise<R>& p, const F& f) : mPromise(p), mCallable(f)
			{
			}

			virtual ~AsyncWorkItem()
			{
			};

			virtual void run()
			{
				mPromise.set_value(mCallable());
			}

		protected:
			promise<R> mPromise;
			F mCallable;
		};


	} // namespace detail

	/*! Used to keep track of threads created by cz::async calls. */
	class AsyncThreadManager : public TSingleton<AsyncThreadManager>
	{
	protected:
		typedef std::vector<thread*, STLAllocator<thread*, GeneralCategoryAllocator> > ThreadList;
		typedef std::deque< detail::AsyncWorkItemBase*, STLAllocator<detail::AsyncWorkItemBase*, GeneralCategoryAllocator> > DequeContainer;
		typedef std::queue<detail::AsyncWorkItemBase*, DequeContainer> WorkQueue;

	public:
		AsyncThreadManager();
		virtual ~AsyncThreadManager();

		template<class R, class F>
		void addWork(
               const promise<R>& p,
               F f // NOTE : Passing as a value, instead of a reference, otherwise async function fails to compile when calling this with a tempoorary object created with "bind"
               )
		{
			// TODO : Allocate using new
			_addWork(CZ_NEW detail::AsyncWorkItem<R,F>(p, f));
		}

		int getNumThreads() const;
		void setNumThreads(int numThreads);
		int getPendingWorkCount() const;
		static void itemConsumer(WorkQueue& workQueue, condition_variable& workAvailable, recursive_mutex& queueMutex, bool &finished);
	private:

		void createThreads();
		void _addWork(detail::AsyncWorkItemBase* item);

		WorkQueue mPendingWork;
		condition_variable mWorkAvailable; // Signals the consumers that we have work available
		ThreadList mThreads;
		mutable recursive_mutex mMutex;
		bool mFinish;
		int mNumThreads;
	};


	template<class F, class A1>
	future< typename CallableTraits<F>::return_type > async(F f, A1 a1)
	{
		promise< typename CallableTraits<F>::return_type > p;
		AsyncThreadManager::getSingleton().addWork(p, bind(f, a1));
		return p.get_future();
	}

	template<class F, class A1, class A2>
	future< typename CallableTraits<F>::return_type > async(F f, A1 a1, A2 a2)
	{
		promise< typename CallableTraits<F>::return_type > p;
		AsyncThreadManager::getSingleton().addWork(p, bind(f, a1, a2));
		return p.get_future();
	}


} // namespace cz

#endif
