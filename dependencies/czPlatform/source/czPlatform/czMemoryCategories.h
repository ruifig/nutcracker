/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com

	purpose:
	Contains all memory categories
	This header file is not guarded by a #indef, because it intentionally needs to be #included more than once
*********************************************************************/

#ifndef CZ_REGISTER_MEMORYCATEGORY
	#define CZ_REGISTER_MEMORYCATEGORY(name) MemCat_##name,
	#define CZ_REGISTERING_MEMCATS_ENUMS
namespace cz{
	enum MemoryCategory{
#endif

	CZ_REGISTER_MEMORYCATEGORY(General)
	CZ_REGISTER_MEMORYCATEGORY(String)
	CZ_REGISTER_MEMORYCATEGORY(Network)

	// Add custom string IDs
	#include "config/czMemoryCategoriesCustom.h"

#ifdef CZ_REGISTERING_MEMCATS_ENUMS
	};
}
#undef CZ_REGISTERING_MEMCATS_ENUMS
#undef CZ_REGISTER_MEMORYCATEGORY
#endif
