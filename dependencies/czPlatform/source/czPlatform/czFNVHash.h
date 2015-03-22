/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com
	
	purpose:

	Hashing, based on  http://www.isthe.com/chongo/tech/comp/fnv/

*********************************************************************/


#ifndef czFNVHash_h__
#define czFNVHash_h__

#include "czPlatformPrerequisites.h"

namespace cz
{

	class FNVHash32
	{
	public:
		FNVHash32() {};
		~FNVHash32() {};
		explicit FNVHash32(cz::u32 val_) : val(val_) {}
		explicit FNVHash32(void* pData, unsigned int dataSize)
		{
			val = compute(pData, dataSize);
		}

		static cz::u32 compute( const void* data, unsigned int dataSize )
		{
			const unsigned char* pData = static_cast<const unsigned char*>(data);
			const unsigned char* pEnd = pData + dataSize;
			unsigned int hashval = 2166136261U;
			unsigned int prime = 16777619U;

			while( pData < pEnd ) 
			{
				hashval *= prime;
				hashval ^= *pData++;
			}

			return hashval;
		}

		inline bool operator< (const FNVHash32& other) const
		{
			return val < other.val;
		}

		inline bool operator> (const FNVHash32& other) const
		{
			return val > other.val;
		}

		inline bool operator== (const FNVHash32& other) const
		{
			return val == other.val;
		}

		cz::u32 val;
	};


} // namespace cz

#endif // czFNVHash_h__

