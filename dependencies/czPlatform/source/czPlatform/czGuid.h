/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com
	
	purpose:
	
*********************************************************************/

#ifndef _czGuid_h_
#define _czGuid_h_

#include "czPlatformPrerequisites.h"

#include "czString.h"


namespace cz
{

	/*! \addtogroup General
		@{
	*/

	/*
		Creates UUID/GUID (Universally unique identifier)
	*/
	class clcpp_attr(reflect,serialize) Guid
	{
	public:
		Guid() : a(0), b(0), c(0), d(0)
		{
		}
		Guid(u32 _a, u32 _b, u32 _c, u32 _d) : a(_a), b(_b), c(_c), d(_d)
		{
		}

		explicit Guid(const char* str);

		static Guid create();

		bool operator==(const Guid& other) const
		{
			return (a==other.a && b==other.b && c==other.c && d==other.d);
		}

		bool operator!=(const Guid& other) const
		{
			return !(operator==(other));
		}

		bool isValid() const
		{
			return (a | b | c | d) !=0;
		}

		void invalidate()
		{
			a = b = c = d = 0;
		}

		String toString() const;		
		bool fromString(const String& str);

		// So it can be used as a key in maps
		bool operator<(const Guid& other) const;

		u32 a,b,c,d;
	};

	/*!
		@}
	*/

} // namespace cz


#endif // _czGuid_h_


