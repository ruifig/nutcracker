/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com
	
	purpose:
*********************************************************************/

#include "czPlatformStableHeaders.h"
#include "czSharedFlag.h"

namespace cz
{

bool SharedFlag::isValid() const
{
	return *m_flag.get();
}

SharedFlagOwner::SharedFlagOwner()
{
	m_flag.m_flag = std::make_shared<bool>(true);
}

// Copy constructor doesn't copy anything. Instead, it creates it's own flag.
// This is because any objects using SharedFlagOwner instances, want their own instances,
// with independent flags.
SharedFlagOwner::SharedFlagOwner(const SharedFlagOwner& other)
{
	m_flag.m_flag = std::make_shared<bool>(true);
}

SharedFlagOwner& SharedFlagOwner::operator=(const SharedFlagOwner& other)
{
	if (&other!=this)
		m_flag.m_flag = std::make_shared<bool>(true);
	return *this;
}


SharedFlagOwner::~SharedFlagOwner()
{
	// Mark as destroyed
	*(m_flag.m_flag.get()) = false;
}

void SharedFlagOwner::explicitSet()
{
	// Mark as destroyed
	*(m_flag.m_flag.get()) = false;
}

SharedFlag SharedFlagOwner::_getFlag()
{
	return m_flag;
}

} // namespace cz

