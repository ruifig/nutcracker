/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com
	
	purpose:
	
*********************************************************************/

#include "czPlatformStableHeaders.h"
#include "czRtti.h"

namespace cz
{

CZ_DEFINE_SINGLETON(cz::RttiManager)

Rtti::Rtti(RttiType rttiType, int size, const char* name, Rtti* baseRtti)
{
	mName = name;
	mType = rttiType;
	mSize = size;
	mBaseRtti = baseRtti;
	mBaseRtti->mDerivedTypes.push_back(this);	
	RttiManager::getSingleton().registerRtti(this);
}

Rtti::~Rtti()
{

}

void Rtti::AddEnum(const char* name, int val)
{
	mEnumPairs.push_back(EnumPair(name, val));
}


//////////////////////////////////////////////////////////////////////////
// RttiManager
//////////////////////////////////////////////////////////////////////////
RttiManager::RttiManager()
{
}

RttiManager::~RttiManager()
{
}

void RttiManager::registerRtti(Rtti* rtti)
{
	if (rtti->getBaseRtti()==NULL)
		mBaselessRtti.push_back(rtti);
}


} // namespace cz
