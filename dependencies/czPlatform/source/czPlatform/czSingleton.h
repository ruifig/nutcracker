/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com

	created:	2012/02/11
	created:	11:2:2012   22:22
	filename: 	C:\Work\crazygaze\czPlatform\inc\czSingleton.h
	file path:	C:\Work\crazygaze\czPlatform\inc
	file base:	czSingleton
	file ext:	h
	
	purpose:
	
*********************************************************************/


#ifndef czSingleton_h__
#define czSingleton_h__

#include "czPlatformPrerequisites.h"
#include "czTicker.h"
#include <memory>
#include <vector>
#include <mutex>

namespace cz
{

/*! \addtogroup General General

	@{
*/

	class BaseSingleton
	{
	public:
		BaseSingleton(){}
		virtual ~BaseSingleton()
		{
		}

		/*!
		 * This is called for each singleton as part of shutdown,
		 * before trying to destroy it.
		 * It can be used to solve shutdown dependencies.
		 */
		virtual void shutdown() {}

		virtual float tick(float /*deltaseconds*/)
		{
			return 0;
		}
	};

	/*
	Keeps tracks of all singletons
	*/
	class SingletonManager
	{
	public:
		SingletonManager();
		~SingletonManager();

		static SingletonManager* getPtr()
		{
			CZ_ASSERT(msOurEntry!=nullptr);
			return msOurEntry;
		}

		template<class T>
		T* create()
		{
			std::shared_ptr<T> p= std::make_shared<T>();
			mSingletons.push_back(SingletonTicker(p, true));
			return p.get();
		}

		void tick(float deltaseconds);

	private:
		static SingletonManager* msOurEntry;
		static bool msDestroyed;
		typedef Ticker< std::shared_ptr<BaseSingleton>, float > SingletonTicker;

		std::vector< SingletonTicker > mSingletons;
	};

	/*! Helper class to implement singletons.

	Singleton classes inherit from this class, simplifying the process of creating
	singletons, and enforcing the singleton rules.
	Creation/deletion method is not enforced, and it's up to you to store the singleton anyway you want,
	as long as you create/destroy only 1 instance.
	To create a singleton, inherit from TSingleton<YourSingletonClass>. Example:

	In your header file:
	\code
	#"/czSingleton.h"

	class MyResourceManager : public TSingleton<MyResourceManager>
	{
		// ... your methods
	};
	\endcode

	In your cpp file:
	\code
	#include "MyResourceManager.h"

	CZ_DEFINE_SINGLETON(MyResourceManager);

	// ... your MyResourceManager methods
	\endcode

	*/
	template< class T >
	class TSingleton : public BaseSingleton
	{
	public:
		TSingleton()
		{
			// Make sure the singleton is not created already
			CZ_ASSERT(!msSingleton);
			msSingleton = static_cast<T*>(this);
			msDestroyed = false;
		}

		virtual ~TSingleton()
		{
			CZ_ASSERT(msSingleton);
			msSingleton = nullptr;
			msDestroyed = true;
		}

		/*! Returns the singleton instance */
		static T& getSingleton()
		{
			if (!msSingleton)
			{
				CZ_ASSERT(!msDestroyed);
				// create singleton
				msSingleton = SingletonManager::getPtr()->create<T>();
				CZ_ASSERT(msSingleton);
			}
			return *msSingleton;
		}

	protected:
		static T* msSingleton;
		static bool msDestroyed;

	private:

		/*! Declared but not defined, since it's a forbidden operation*/
		TSingleton( const TSingleton<T> &);

		/*! Declared but not defined, since it's a forbidden operation*/
		TSingleton& operator=(const TSingleton<T> &);

	};



/*! Helper macro to define the singleton pointer.
This macro needs to be used inside the same namespace where you have your singleton.
*/
#define CZ_DEFINE_SINGLETON(SingletonClass) \
	template<> SingletonClass* TSingleton<SingletonClass>::msSingleton=NULL; \
	template<> bool TSingleton<SingletonClass>::msDestroyed=false;




/*!
	@}
*/


} // namespace cz


#endif // czSingleton_h__
