/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com
	
	purpose:
	
*********************************************************************/

#ifndef _czRtti_h_
#define _czRtti_h_

#include "czPlatformPrerequisites.h"
#include "czString.h"
#include "czArray.h"
#include "czSingleton.h"

namespace cz
{

	enum RttiType
	{
		kRttiTypeNative,
		kRttiTypeEnum,
		kRttiTypePOD,
		kRttiTypePolymorphic
	};

	class Rtti;

	template<typename T>
	Rtti* getRttiOf();

	class Rtti
	{
	public:

		Rtti(RttiType rttiType, int size, const char* name, Rtti* baseRtti);
		~Rtti();

		template<class C, typename T>
		void AddField(const char* name, T C::*p)
		{
			size_t offset = *((size_t*)&(p));
			mFields.push_back(Field(name, getRttiOf<T>(), offset, false));
		}
		// Specialization to add a pointer field
		template<class C, typename T>
		void AddField(const char* name, T* C::*p)
		{
			size_t offset = *((size_t*)&(p));
			mFields.push_back(Field(name, getRttiOf<T>(), offset, true));
		}

		void AddEnum(const char* name, int val);
	public:
		
		struct Field
		{
		public:
			Field(const String& name_, const Rtti* rtti_, size_t offset_, bool isPointer_)
			{
				name = name_;
				typeRtti = rtti_;
				offset = offset_;
				flags = 0;
				isPointer = isPointer_;
			}
			size_t offset;
			String name;
			const Rtti* typeRtti;
			bool isPointer;
			u32 flags;
		};

		struct EnumPair
		{
			EnumPair() : value(0), flags(0)	{ }
			EnumPair(const char* name_, int value_) : name(name_), value(value_), flags(0) {}
			String name;
			int value;
			u32 flags;
		};

		const Rtti* getBaseRtti() const
		{
			return mBaseRtti;
		}

	protected:
		friend class RttiManager;
		RttiType mType;
		u32 mSize;
		Rtti* mBaseRtti;
		TArray<Rtti*> mDerivedTypes;
		String mName;
		TArray<Field> mFields;
		TArray<EnumPair> mEnumPairs;

	};

	class RttiManager : public TSingleton<RttiManager>
	{
	public:
		RttiManager();
		virtual ~RttiManager();

	protected:
		friend class Rtti;
		void registerRtti(Rtti* rtti);

	private:
		TArray<Rtti*> mBaselessRtti; // Array with top level Rtti objects. Other rtti objects can be accessed through these ones
	};



} // namespace cz

#endif // _czRtti_h_
