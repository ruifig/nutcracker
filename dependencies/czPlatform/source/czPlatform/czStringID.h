/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com
	
	purpose:
	
*********************************************************************/

#ifndef czStringID_h__
#define czStringID_h__

#include "czString.h"
#include "czStringIDs.h"
#include "czArray.h"

enum
{
	IDFLAG_Suppressed=0x00000001 // Logging is suppressed for names with this flag
};

namespace cz
{

	class StringID
	{
	public:
		static StringID create(const char* str);

		bool operator==(const StringID& other) const
		{
			return mIndex==other.mIndex;
		}

		const String& getString() const
		{
			return msEntries[mIndex].mString;
		}

		static const String& getString(etStringID id)
		{
			return msEntries[id].mString;
		}

		StringID(etStringID id)
		{
			mIndex = id;
		}

		void setFlags(u32 flags)
		{
			msEntries[mIndex].mFlags |= flags;
		}

		void clearFlags(u32 flags)
		{
			msEntries[mIndex].mFlags &= ~flags;
		}

		bool hasAnyFlag(u32 flags)
		{
			return (msEntries[mIndex].mFlags & flags) !=0;
		}

		bool hasAllFlags(u32 flags)
		{
			return (msEntries[mIndex].mFlags & flags)==flags;
		}

		static void _initialize();
		static void _destroy();
	private:

		StringID()
		{
			mIndex = 0;
		}

		struct Entry
		{
			Entry()
			{
				mID = 0;
				mFlags = 0;
			}

			~Entry()
			{
			
			}

			u32 mID; // hashed string
			u32 mFlags;
			String mString;
			bool operator==(const Entry& other) const
			{
				return mID==other.mID;
			}
		};

		int mIndex;

		// Array with all the IDs
		static TArray<StringID::Entry> msEntries;
	};

} // namespace cz


#endif // czStringID_h__
