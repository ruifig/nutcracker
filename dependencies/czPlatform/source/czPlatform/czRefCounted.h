/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com
	
	purpose:
	
*********************************************************************/


#ifndef czRefCounted_h__
#define czRefCounted_h__


#include "czPlatformPrerequisites.h"

namespace cz
{

	template<class T>
	class TSharedPtr
	{
	public:

		TSharedPtr()
		{
			mPtr = NULL;
		}

		TSharedPtr(const TSharedPtr &p)
		{
			mPtr = const_cast<T*>(p.get());
			if (mPtr)
				mPtr->incRef();
		}

		template<class Y>
		TSharedPtr(const TSharedPtr<Y> &p)
		{
			mPtr = static_cast<T*>(const_cast<Y*>(p.get()));
			if (mPtr)
				mPtr->incRef();
		}

		explicit TSharedPtr(T* p)
		{
			mPtr = p;
			if (mPtr)
				mPtr->incRef();
		}

		TSharedPtr& operator=(T* p)
		{
			if (mPtr!=p)
			{
				if (mPtr)
					mPtr->decRef();
				mPtr = p;
				if (mPtr)
					mPtr->incRef();
			}

			return *this;
		}

		template<class Y>
		TSharedPtr& operator=(Y* p)
		{
			return operator=(static_cast<T*>(p));
		}

		TSharedPtr& operator=(const TSharedPtr& p)
		{
			if (mPtr!=p.get())
			{
				if (mPtr)
					mPtr->decRef();
				mPtr = const_cast<T*>(p.get());
				if (mPtr)
					mPtr->incRef();
			}

			return *this;
		}

		template<class Y>
		TSharedPtr& operator=(const TSharedPtr<Y>& p)
		{
			if (mPtr!=p.get())
			{
				if (mPtr)
					mPtr->decRef();
				mPtr = static_cast<T*>(const_cast<Y*>(p.Get()));
				if (mPtr)
					mPtr->incRef();
			}
			return *this;
		}

		~TSharedPtr()
		{
			if (mPtr)
				mPtr->decRef();
		}

		void release()
		{
			if (mPtr)
			{
				mPtr->decRef();
				mPtr = NULL;
			}
		}

		T* operator->()
		{
			CZ_ASSERT(mPtr!=0);
			return mPtr;
		}

		const T* operator->() const
		{
			return mPtr;
		}

		const T& operator*() const
		{
			CZ_ASSERT(mPtr!=0);
			return mPtr;
		}

		T* get() const
		{
			return mPtr;
		}

		operator bool () const
		{
			return !(mPtr==NULL);
		}

		bool operator == (const TSharedPtr &other) const
		{
			return (other.get()==this->mPtr);
		}

		template<class Y>
		bool operator == (const TSharedPtr<Y> &other) const
		{
			return (other.get()==this->mPtr);
		}

	private:
		T* mPtr;
	};


	template<typename T>
	class TRefCounted
	{
	public:

		void incRef()
		{
			mRefCount++;
		}

		void decRef()
		{
			if (mRefCount>0)
				mRefCount--;

			if (mRefCount<=0)
			{
				T* ptr = static_cast<T*>(this);
				CZ_DELETE ptr;
			}
		}

		int getRefCount()
		{
			return mRefCount;
		}

		virtual ~TRefCounted()
		{
		}

	protected:

		TRefCounted()
		{
			mRefCount = 0;
		}

	private:

		int mRefCount;
	};


} // namespace cz

#endif
