/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com

	created:	2012/03/10
	created:	10:3:2012   17:14
	filename: 	czThread.h

	purpose:
	Include code equivalent to what you can find in C++1 <thread> header
*********************************************************************/

#ifndef czThread_h__
#define czThread_h__

#include "czPlatformPrerequisites.h"

#include "czAtomicCount.h"
#include "czFunctional.h"

#include "czThreadCommon.h"

#if CZ_PLATFORM == CZ_PLATFORM_WIN32
	#include "detail/czThread_win32_hpp.h"
#elif CZ_HAS_PTHREADS
    #include "detail/czThread_pthreads_hpp.h"
#else
	#error Not implemented
#endif

namespace cz
{

	/*! \addtogroup Threading
		@{
	*/

	// Forward declaration
	namespace detail
	{

		struct thread_data_base;
		void intrusive_ptr_add_ref(thread_data_base * p);
		void intrusive_ptr_release(thread_data_base * p);

		struct thread_data_base : public detail::thread_data_implementation, public TAllocatedObject<GeneralCategoryAllocator>
		{
			atomic_count count;
			//unsigned int id;

			thread_data_base() : count(0)
			{
			}

			virtual ~thread_data_base()
			{}

			friend void intrusive_ptr_add_ref(thread_data_base* p)
			{
				p->count.inc();
			}

			friend void intrusive_ptr_release(thread_data_base* p)
			{
				if (p->count.dec_and_test())
				{
					CZ_DELETE p;
				}
			}

		};

		typedef intrusive_ptr<detail::thread_data_base> thread_data_ptr;

		template<class F>
		struct thread_data : public thread_data_base
		{
			thread_data(F f_) : f(f_)
			{
			}

			virtual void run()
			{
				// Create a smart pointer without incrementing reference counter,
				// so that I get the release to the add_ref done in start_thread
				detail::thread_data_ptr thread_info( this , false);

				// Run the user callable
				f();
			}

			F f;
		};

	}


	/*! Similar to boost this_thread. */
	namespace this_thread
	{
		inline void yield()
		{
			return detail::thread_implementation::_yield();
		}
		/*! Halts the current thread by the specified milliseconds. */
		inline void sleep_ms(int milliseconds)
		{
			return detail::thread_implementation::_sleep_ms(milliseconds);
		}
	}


	/*! Cut-down version of threads from boost/C++11. */
	class thread
	{
	private:
	public:

		/*! Construct an empty instance (not running any thread yet). */
		thread()
		{
		}

		template<class F>
		explicit thread(F f) : thread_info(make_thread_info(f))
		{
			start_thread();
		}

		template<class F, class A1>
		explicit thread(F f, A1 a1) : thread_info(make_thread_info(::cz::bind(f, a1)))
		{
			start_thread();
		}

		template<class F, class A1, class A2>
		explicit thread(F f, A1 a1, A2 a2) : thread_info(make_thread_info(::cz::bind(f, a1, a2)))
		{
			start_thread();
		}

		template<class F, class A1, class A2, class A3>
		explicit thread(F f, A1 a1, A2 a2, A3 a3) : thread_info(make_thread_info(::cz::bind(f, a1, a2, a3)))
		{
			start_thread();
		}

		template<class F, class A1, class A2, class A3, class A4>
		explicit thread(F f, A1 a1, A2 a2, A3 a3, A4 a4) : thread_info(make_thread_info(::cz::bind(f, a1, a2, a3, a4)))
		{
			start_thread();
		}

		template<class F, class A1, class A2, class A3, class A4, class A5 >
		explicit thread(F f, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5) : thread_info(make_thread_info(::cz::bind(f, a1, a2, a3, a4, a5)))
		{
			start_thread();
		}

		template<class F, class A1, class A2, class A3, class A4, class A5, class A6 >
		explicit thread(F f, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6) : thread_info(make_thread_info(::cz::bind(f, a1, a2, a3, a4, a5, a6)))
		{
			start_thread();
		}

		template<class F, class A1, class A2, class A3, class A4, class A5, class A6, class A7 >
		explicit thread(F f, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7) : thread_info(make_thread_info(::cz::bind(f, a1, a2, a3, a4, a5, a6, a7)))
		{
			start_thread();
		}

		template<class F, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8 >
		explicit thread(F f, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8) : thread_info(make_thread_info(::cz::bind(f, a1, a2, a3, a4, a5, a6, a7, a8)))
		{
			start_thread();
		}

		template<class F, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9 >
		explicit thread(F f, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9) : thread_info(make_thread_info(::cz::bind(f, a1, a2, a3, a4, a5, a6, a7, a8, a9)))
		{
			start_thread();
		}

		void _setpriority(thread_priority priority)
		{
			detail::thread_implementation::_setpriority(thread_info.get(), priority);
		}

		void _setaffinity(u32 mask)
		{
			detail::thread_implementation::_setaffinity(thread_info.get(), mask);
		}

		bool joinable() const;
		void join();
		void detach();
		void yield()
		{
			this_thread::yield();
		}
		void sleep_ms(int milliseconds)
		{
			this_thread::sleep_ms(milliseconds);
		}

		~thread();

		void swap(thread& other);

	private:

		// Not permitted
		thread& operator=(thread& other);

		void release_handle();
		detail::thread_data_ptr get_thread_info () const;
		void start_thread();

		template< typename F >
		static inline detail::thread_data_ptr make_thread_info(F f)
		{
			return detail::thread_data_ptr(CZ_NEW detail::thread_data<F>(f));
		}

		detail::thread_data_ptr thread_info;
	};


	/*!
		@}
	*/

} // namespace cz

#endif // czThread_h__
