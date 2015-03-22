/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com

	created:	2012/03/02
	created:	2:3:2012   2:54
	filename: 	czFunction.h
	
	purpose:
	
*********************************************************************/


#ifndef czFunction_h__
#define czFunction_h__

#include "czPlatformPrerequisites.h"
#include "czBind.h"

namespace cz
{
	/*

	template<class R, class A1, class A2>
	struct Function_base
	{
		typedef bindst<R,A1,A2> bind_type;
		typedef R return_type;
		bind_type b;

		Function_base() {}
		Function_base(const bind_type& b_) : b(b_) {}

		operator bool() const
		{
			return (b.f!=NULL) ? true : false;
		}
	};

	template<class R, class A1=void, class A2=void>
	struct Function : public Function_base<R,A1,A2>
	{
		Function () {}
		Function(const bind_type& b_) : Function_base(b_) {}
		R operator() ()
		{
			return b.f(b.a1,b.a2);
		}
	};

	template<class R, class A1>
	struct Function<R,A1,void> : public Function_base<R,A1,void>
	{
		Function () {}
		Function(const bind_type& b_) : Function_base(b_) {}
		R operator() ()
		{
			return b.f(b.a1);
		}
	};

	template<class R>
	struct Function<R,void,void> : public Function_base<R, void,void>
	{
		Function () {}
		Function(const bind_type& b_) : Function_base(b_) {}
		R operator() ()
		{
			return b.f();
		}
	};

	template<>
	struct Function<void, void, void> : public Function_base<void,void,void>
	{
		Function () {}
		Function(const bind_type& b_) : Function_base(b_) {}
		void operator() ()
		{
			b.f();
		}
	};
	*/


	template<class F>
	class Function
	{
	public:

	private:
	};

	/*
	template<class R>
	class Future
	{
	private:
		struct DataBase
		{
			DataBase() {};
			virtual ~DataBase() {};
			virtual void execute(R& r)=0;
		};

		template<class F>
		struct Data : public DataBase
		{
			Data(const F& f_) : f(f_)
			{
			}
			virtual ~Data() {};
			F f;
			virtual void execute(R& r)
			{
				r = f();
			}
		};

	public:

		bool valid() const
		{
			return mDone;
		}

		R& get()
		{
			return mResult;
		}

		template<class F>
		explicit Future(const F& f)
		{
			mDone = false;
			mData = new Data<F>(f);
		}

		~Future()
		{
			if (mData)
				delete mData;
		}

		Future()
		{
			mDone = false;
			mData = NULL;
		}
		Future(const Future& other)
		{
			getFrom(other);
		}

		Future& operator= (const Future& other)
		{
			if (this!=&other)
				getFrom(other);
			return *this;
		}

		R& _execute()
		{
			mData->execute(mResult);
			mDone = true;
			return mResult;
		}
	private:

		void getFrom(const Future& other)
		{
			if (mData)
				delete mData;
			mData = other.mData;
			other.mData = NULL;
			mResult = other.mResult;
			mDone = other.mDone;
		}

		mutable DataBase* mData;
		R mResult;
		bool mDone;
	};

	*/


} // namespace cz


#endif // czFunction_h__
