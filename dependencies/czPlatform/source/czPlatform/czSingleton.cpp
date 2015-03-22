#include "czPlatformStableHeaders.h"
#include "czSingleton.h"

namespace cz
{

SingletonManager* SingletonManager::msOurEntry=nullptr;
bool SingletonManager::msDestroyed = false;


SingletonManager::SingletonManager()
{
	CZ_ASSERT(msOurEntry==nullptr);
	msOurEntry = this;
	msDestroyed = false;

	// TODO : This is to avoid a crash in Ticker when calling Singleton::tick, where ticking some singletons causes another Singleton to be created,
	// therefore reallocating the vector, deleting all the Ticker objects.
	// This causes an access violation when Ticker is done with the singleton tick and tries to update some state before
	// returning
	mSingletons.reserve(128);
}

SingletonManager::~SingletonManager()
{
	CZ_ASSERT(msOurEntry);

	for (size_t i=0; i<mSingletons.size(); i++)
		mSingletons[i].getObj()->shutdown();

	while(mSingletons.size())
	{
		auto p = mSingletons.back();
		mSingletons.pop_back();
	}
	msOurEntry=nullptr;
	msDestroyed = true;
}


void SingletonManager::tick(float deltaseconds)
{
	// Not using iterators because ticking unknown code can cause other singletons to be created
	// and invalidate the iterators
	for(size_t i=0; i<mSingletons.size(); i++)
		mSingletons[i].tick(deltaseconds);
}

} // namespace cz
