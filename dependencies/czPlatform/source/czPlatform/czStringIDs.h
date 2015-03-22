/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com

	purpose:
	Contains all the hardcoded string IDs, so we can access them with an enum
	This header file is not guarded by a #indef, because it intentionally needs to be #included more than once
*********************************************************************/

#ifndef CZ_REGISTER_STRINGID
	#define CZ_REGISTER_STRINGID(name) ID_##name,
	#define CZ_REGISTERING_STRINGID_ENUMS
namespace cz{
	enum etStringID{
#endif

	CZ_REGISTER_STRINGID(Log)
	CZ_REGISTER_STRINGID(Memory)
	CZ_REGISTER_STRINGID(Network)

	// Add custom string IDs
	#include "config/czStringIDsCustom.h"

#ifdef CZ_REGISTERING_STRINGID_ENUMS
	};
}
#undef CZ_REGISTERING_STRINGID_ENUMS
#undef CZ_REGISTER_STRINGID
#endif
