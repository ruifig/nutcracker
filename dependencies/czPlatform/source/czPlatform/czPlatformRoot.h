/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com

	created:	2012/02/10
	created:	10:2:2012   0:34
	filename: 	C:\Work\crazygaze\czPlatform\src\czPlatformRoot.h
	file path:	C:\Work\crazygaze\czPlatform\src
	file base:	czPlatformRoot
	file ext:	h
	
	purpose:
	Exposes several tweaks
*********************************************************************/

 /** @file */

#ifndef czPlatformRoot_h__
#define czPlatformRoot_h__

#include "czPlatformPrerequisites.h"

namespace cz
{
	// Forward declarations
#if CZ_MEMORY_TRACKER_ENABLED
	class MemoryTracker;
	class LoggerOutput;
#endif

	// Forward declaration
	struct PlatformRootSystems;

	/*! czPlatform Root object, from where you can access several systems.
	*/
	class PlatformRoot : public TSingleton<PlatformRoot>
	{
	public:

		/*! Used to specify some overrides/tweaks when initialising \link PlatformRoot \endlink
		*/
		struct Config
		{
			/*! Object to use for allocating memory. If NULL, it will use a default implementation (e.g: malloc/free).
			Set to NULL to use the default.
			*/
			ApplicationMemoryAllocator* allocator;
			
			/*! How many threads to create to use with \link cz::async \endlink.
			 If 0, it will automatically pick a value depending on the platform.
			 */
			int workerThreads;

			/*
			This is enabled by default even for UI applications.
			*/
			bool createConsole;
		};

		/*! Returns the platform default config.
		*/
		static void getDefaultConfig(Config* config);

		/*! Constructor
		\param config Lets you provide some hooks, tweaks and such to configure the system to your needs
		*/
		PlatformRoot(const Config* config=NULL);

		virtual ~PlatformRoot();

		/*! Returns the lower level allocator */
		ApplicationMemoryAllocator* getAppAllocator()
		{
			return mAllocator;
		}

		void tick(double *pCalculatedDeltaSeconds=nullptr);

		LoggerOutput* getDefaultLogOutput();

	private:
		ApplicationMemoryAllocator* mAllocator;

#if CZ_MEMORY_TRACKER_ENABLED
		MemoryTracker* mMemoryTracker;
#endif

		PlatformRootSystems* mSystems;
	};
}

#endif // czPlatformRoot_h__
