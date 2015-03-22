/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com
	
	purpose:
	
*********************************************************************/


#ifndef _czIniFile_h_
#define _czIniFile_h_

#include "czPlatformPrerequisites.h"
#include "czFilename.h"
#include "czString.h"
#include "czArray.h"
#include "czMemory.h"

namespace cz
{

	class IniFile : TAllocatedObject<GeneralCategoryAllocator>
	{
	public:

		class Entry;
		
		class Section : public TAllocatedObject<GeneralCategoryAllocator>
		{
		public:
			Section()
			{
			}
			~Section()
			{
			}

			const String& getName() const
			{
				return mName;
			}

			int getNumEntries() const
			{
				return mEntries.size();
			}

			Entry* getEntry(int index)
			{
				return &mEntries[index];
			}

			Entry* getEntry(const char* name, bool bCreate=true);
			/*
			Entry* getEntryWithDefault(const char* name, const char* defaultValue);
			Entry* getEntryWithDefault(const char* name, bool defaultValue);
			Entry* getEntryWithDefault(const char* name, int defaultValue);
			Entry* getEntryWithDefault(const char* name, float defaultValue);
			*/
			template<typename T>
			Entry* getEntryWithDefault(const char* name, T defaultValue)
			{
				auto entry = getEntry(name);
				if (entry->mValue.size() == 0)
					entry->setValue(defaultValue);
				return entry;
			}

			//! Changes the value of an existing entry, or creates a new one if necessary
			void setValue(const char* szEntryName, const char* szValue);
			//! Changes the value of an existing entry, or creates a new one if necessary
			void setValue(const char* szEntryName, int val);
			//! Changes the value of an existing entry, or creates a new one if necessary
			void setValue(const char* szEntryName, float val);

			//! Adds a new entry/value pair, even if it creates a duplicated entry name
			void add(const char* szEntryName, const char* szValue);
			//! Adds a new entry/value pair, even if it creates a duplicated entry name
			void add(const char* szEntryName, int val);
			//! Adds a new entry/value pair, even if it creates a duplicated entry name
			void add(const char* szEntryName, float val);

			/*
			const String& getValueAsString(const char* szEntryName, const char* defaultVal);
			bool getValueAsBoolean(const char* szEntryName, bool defaultVal);
			int getValueAsInt(const char* szEntryName, int defaultVal);
			float getValueAsFloat(const char* szEntryName, float defaultVal);
			*/

		protected:
			friend class IniFile;
			void init(const char* name);

		private:
			String mName;
			TArray<Entry> mEntries;
		};


		class Entry
		{
		public:
			Entry()
			{
			}

			Entry(const Entry& other)
				: mName(other.mName), mValue(other.mValue)
			{
			}

			~Entry()
			{
			}


			const String& getName() const
			{
				return mName;
			}

			const String& asString() const
			{
				return mValue;
			}

			int asInt() const;
			float asFloat() const;
			bool asBoolean() const;

			template<typename T> T as();
			template<> int as()
			{
				return asInt();
			}
			template<> bool as()
			{
				return asBoolean();
			}
			template<> float as()
			{
				return asFloat();
			}
			template<> const char* as()
			{
				return asString().c_str();
			}
			template<> String as()
			{
				return asString();
			}


			bool operator==(const Entry& other) const
			{
				return mName==other.mName;
			}

		protected:
			friend class IniFile::Section;
			void init(const char* name, const char* val);
			void setValue(const char* val);
			void setValue(bool val);
			void setValue(int val);
			void setValue(float val);


		private:
			String mName;
			String mValue;
		};

		IniFile() {}
		virtual ~IniFile();
		bool open(const char* filename);

		int getNumSections() const
		{
			return mSections.size();
		}

		Section* getSection(int index)
		{
			return mSections[index];
		}

		Section* getSection(const char* szName, bool bCreate=true);

		template<typename T>
		T getValue(const char* szSection, const char* szName, T defaultVal)
		{
			return getSection(szSection)->getEntryWithDefault(szName, defaultVal)->as<T>();
		}

	private:
		
		TArray<Section*> mSections;
	};

} // namespace cz

#endif // _czIniFile_h_


