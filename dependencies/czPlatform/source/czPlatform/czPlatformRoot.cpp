/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com

	created:	2012/02/10
	created:	10:2:2012   0:44
	filename: 	C:\Work\crazygaze\czPlatform\src\czPlatformRoot.cpp
	file path:	C:\Work\crazygaze\czPlatform\src
	file base:	czPlatformRoot
	file ext:	cpp
	
	purpose:
*********************************************************************/

#include "czPlatformStableHeaders.h"

#include "czMemoryTracker.h"
#include "czFilesystem.h"
#include "czFuture.h"
#include "czStringID.h"
#include "czLogger.h"
#include "czRtti.h"
#include "czTCPSocket.h"
#include "czTimer.h"

#if CZ_PLATFORM == CZ_PLATFORM_WIN32
	#include "czWindowsConsole.h"
#endif

namespace cz
{

namespace
{
	// Default low level memory allocator
	class DefaultApplicationMemoryAllocator : public ApplicationMemoryAllocator
	{
	public:
		virtual void* allocate(size_t size, const char* /*file*/, int /*line*/, const char* /*func*/)
		{
			return malloc(size);
		}

		virtual void deallocate(void* ptr)
		{
			free(ptr);
		}
	} gDefaultAllocator;
}


CZ_DEFINE_SINGLETON(PlatformRoot)

struct PlatformRootSystems : TAllocatedObject<GeneralCategoryAllocator>
{
	cz::HighResolutionTimer mTickTimer;
	scoped_ptr<LoggerOutput> mDefaultLogOutput;
	SingletonManager mSingletonManager;
	//Logger mLogger;
	//Filesystem mFilesystem;
	//TCPSocketManager mTCPSocketManager;
	//AsyncThreadManager mAsyncThreadManager;
	//RttiManager mRttiManager;
};

void PlatformRoot::getDefaultConfig(Config* config)
{
	memset(config, 0, sizeof(*config));
	config->allocator = &gDefaultAllocator;

#if CZ_PLATFORM == CZ_PLATFORM_WIN32
	{
		SYSTEM_INFO sysinfo;
		GetSystemInfo( &sysinfo );
		config->workerThreads = sysinfo.dwNumberOfProcessors;
	}
#else
	config->workerThreads = 1;
#endif

	config->createConsole = true;
}

PlatformRoot::PlatformRoot(const Config* config)
{
#if CZ_MEMORY_TRACKER_ENABLED
	static MemoryTracker memTracker;
	mMemoryTracker = &memTracker;

	// Generate registration of all categories
	#define CZ_REGISTER_MEMORYCATEGORY(name) mMemoryTracker->registerCategory(MemCat_##name, #name);
	#include "czMemoryCategories.h"

#endif

	Config cfg;
	Config defaultcfg;
	getDefaultConfig(&defaultcfg);
	if (config)
		cfg = *config;
	else
		cfg = defaultcfg;

	if (!cfg.allocator)
		cfg.allocator = defaultcfg.allocator;
	if (cfg.workerThreads==0)
		cfg.workerThreads = defaultcfg.workerThreads;

	CZ_ASSERT(cfg.allocator!=NULL);
	CZ_ASSERT(cfg.workerThreads!=0);

	// This needs to be before any allocations
	mAllocator = cfg.allocator;

	// Initialize all hardcoded string ids
	StringID::_initialize();

	mSystems = CZ_NEW PlatformRootSystems;
	// Manually cause ordered creation of some singletons
	Logger::getSingleton();
	Filesystem::getSingleton();


	// Create default log output
#if CZ_LOG_ENABLED
#if CZ_PLATFORM == CZ_PLATFORM_WIN32
	if (cfg.createConsole)
	{
		WindowsConsole* console = CZ_NEW WindowsConsole(100,30,110,1000);
		console->enableUTF8Support();
		mSystems->mDefaultLogOutput.reset( console );
	}
#endif
#endif

	if (mSystems->mDefaultLogOutput)
		Logger::getSingleton().addOutput(mSystems->mDefaultLogOutput.get());

	Logger::getSingleton().log(ID_Log, Logger::LOG_INFO, "Platform root object initialized");

	AsyncThreadManager::getSingleton().setNumThreads( (cfg.workerThreads) ? cfg.workerThreads : 1);
	Filesystem::getSingleton().setCWDToDefault();

	mSystems->mTickTimer.reset();
}

PlatformRoot::~PlatformRoot()
{
	CZ_DELETE mSystems;
	StringID::_destroy();
}

void PlatformRoot::tick(double *pCalculatedDeltaSeconds)
{
	double delta = mSystems->mTickTimer.seconds();
	
	// The reset needs to be here before the ticking, otherwise, the delta calculation will be wrong
	mSystems->mTickTimer.reset();

	// Save this to pass to the caller, if requested
	if (pCalculatedDeltaSeconds)
		*pCalculatedDeltaSeconds = delta;

	mSystems->mSingletonManager.tick(static_cast<float>(delta));
}

LoggerOutput* PlatformRoot::getDefaultLogOutput()
{
	return mSystems->mDefaultLogOutput.get();
}

} // namespace cz
