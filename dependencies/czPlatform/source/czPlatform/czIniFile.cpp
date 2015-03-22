/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com
	
	purpose:
	
*********************************************************************/

#include "czPlatformStableHeaders.h"
#include "czIniFile.h"
#include "czLogger.h"
#include "czString.h"
#include <sstream>
#include <czPlatform/czFile.h>

namespace cz
{

	//////////////////////////////////////////////////////////////////////////
	// Entry
	//////////////////////////////////////////////////////////////////////////
	void IniFile::Entry::init( const char* name, const char* val )
	{
		mName = name;
		mValue = val;
	}

	void IniFile::Entry::setValue( const char* val )
	{
		mValue = val;
	}

	void IniFile::Entry::setValue(bool val)
	{
		mValue = val ? "true" : "false";
	}

	void IniFile::Entry::setValue(int val)
	{
		std::ostringstream ostr;
		ostr << val;
		mValue = ostr.str().c_str();
	}

	void IniFile::Entry::setValue(float val)
	{
		std::ostringstream ostr;
		ostr << val;
		mValue = ostr.str().c_str();
	}

	int IniFile::Entry::asInt() const
	{
		int val = atoi(mValue.c_str());
		return val;
	}

	bool IniFile::Entry::asBoolean() const
	{
		if (mValue=="1" || mValue=="true" || mValue=="True" || mValue=="TRUE")
			return true;
		else
			return false;
	}

	float IniFile::Entry::asFloat() const
	{
		return static_cast<float>(atof(mValue.c_str()));
	}

	//////////////////////////////////////////////////////////////////////////
	// Section
	//////////////////////////////////////////////////////////////////////////

	void IniFile::Section::init(const char* name)
	{
		mName = name;
	}

	IniFile::Entry* IniFile::Section::getEntry(const char* name, bool bCreate)
	{
		for (int  i=0; i<mEntries.size(); i++)
		{
			if (mEntries[i].getName() == name)
				return &mEntries[i];
		}

		if (bCreate)
		{
			mEntries.push(IniFile::Entry());
			mEntries.last().init(name, "");
			return &mEntries.last();
		}
		else
		{
			return NULL;
		}

	}

	/*
	IniFile::Entry* IniFile::Section::getEntryWithDefault(const char* name, const char* defaultValue)
	{
		auto entry = getEntry(name);
		if (entry->mValue.size()==0)
			entry->setValue(defaultValue);
		return entry;
	}

	IniFile::Entry* IniFile::Section::getEntryWithDefault(const char* name, bool defaultValue)
	{
		auto entry = getEntry(name);
		if (entry->mValue.size()==0)
			entry->setValue(defaultValue);
		return entry;
	}
	IniFile::Entry* IniFile::Section::getEntryWithDefault(const char* name, int defaultValue)
	{
		auto entry = getEntry(name);
		if (entry->mValue.size()==0)
			entry->setValue(defaultValue);
		return entry;
	}
	IniFile::Entry* IniFile::Section::getEntryWithDefault(const char* name, float defaultValue)
	{
		auto entry = getEntry(name);
		if (entry->mValue.size()==0)
			entry->setValue(defaultValue);
		return entry;
	}
	*/

	void IniFile::Section::setValue(const char* szEntryName, const char* szValue)
	{
		IniFile::Entry* pEntry = getEntry(szEntryName);
		pEntry->setValue(szValue);
	}

	void IniFile::Section::setValue(const char* szEntryName, int val)
	{
		IniFile::Entry* pEntry = getEntry(szEntryName);
		pEntry->setValue(val);
	}

	void IniFile::Section::setValue(const char* szEntryName, float val)
	{
		IniFile::Entry* pEntry = getEntry(szEntryName);
		pEntry->setValue(val);
	}

	void IniFile::Section::add(const char* szEntryName, const char* szValue)
	{
		mEntries.push(IniFile::Entry());
		mEntries.last().init(szEntryName, szValue);
	}
	void IniFile::Section::add(const char* szEntryName, int val)
	{
		mEntries.push(IniFile::Entry());
		mEntries.last().init(szEntryName, "");	
		mEntries.last().setValue(val);
	}
	void IniFile::Section::add(const char* szEntryName, float val)
	{
		mEntries.push(IniFile::Entry());
		mEntries.last().init(szEntryName, "");	
		mEntries.last().setValue(val);
	}

	/*
	const String& IniFile::Section::getValueAsString(const char* szEntryName, const char* defaultVal)
	{
		IniFile::Entry* pEntry = getEntry(szEntryName);
		return pEntry->asString();
	}

	bool IniFile::Section::getValueAsBoolean(const char* szEntryName)
	{
		IniFile::Entry* pEntry = getEntry(szEntryName);
		return pEntry->asBoolean();
	}

	int IniFile::Section::getValueAsInt(const char* szEntryName)
	{
		IniFile::Entry* pEntry = getEntry(szEntryName);
		return pEntry->asInt();
	}

	float IniFile::Section::getValueAsFloat(const char* szEntryName)
	{
		IniFile::Entry* pEntry = getEntry(szEntryName);
		return pEntry->asFloat();
	}
	*/

	//////////////////////////////////////////////////////////////////////////
	// IniFile
	//////////////////////////////////////////////////////////////////////////

	IniFile::~IniFile()
	{
		while (mSections.size())
		{
			CZ_DELETE mSections.last();
			mSections.pop();
		}
	}

	bool IniFile::open(const char* filename)
	{
		File file;
		if (!file.try_open(filename, File::FILEMODE_READ))
		{
			czWarning(ID_Log, "Error opening ini file %s", filename);
			return false;
		}

		int size = file.size();
		char* text = static_cast<char*>(CZ_ALLOC(size+1, MemCat_General));
		file.read(text, size);
		text[size]=0;
		std::vector<String> lines;
		stringSplitIntoLines(text, size, &lines);
		CZ_FREE(text, MemCat_General);

		String tmp1, tmp2, tmp3;

		for(int i=0; i<(int)lines.size(); i++)
		{
			String& line = lines[i];
			line = trim(line);
			String::iterator it;

			if (line.c_str()[0]==';' || line.c_str()[0]=='#' )
			{
				// Its a comment
			}
			else if (line.c_str()[0]=='[') // its a section
			{
				tmp1 = "";
				tmp1.append(line.begin()+1, line.end()-1);
				tmp1 = trim(tmp1);
				Section* section = CZ_NEW Section();
				section->init(tmp1.c_str());
				mSections.push(section);
			}
			else if ( (it=std::find(line.begin(), line.end(), '='))!=line.end()) // its a value
			{
				if (mSections.size())
				{
					tmp1 = "";
					tmp1.append(line.begin(), it);
					tmp1 = trim(tmp1);
					tmp2 = "";
					tmp2.append(it+1,line.end());
					tmp2 = trim(tmp2);
					if (*tmp2.begin()=='"' || *tmp2.begin()=='\'')
					{
						tmp3 = "";
						tmp3.append(tmp2.begin()+1, tmp2.end()-1);
						mSections.last()->add(tmp1.c_str(), tmp3.c_str());
					}
					else
					{
						mSections.last()->add(tmp1.c_str(), tmp2.c_str());
					}

				}
				else
				{
					czWarning(ID_Log, "Value with no section in INI File (%s) : %s ", filename, line.c_str());
				}
			}
			else if (line=="")
			{

			}
			else
			{
				czWarning(ID_Log, "Invalid line in INI File (%s) : %s ", filename, line.c_str());
			}
		}

		return true;
	}

	IniFile::Section* IniFile::getSection(const char* szName, bool bCreate)
	{
		for (int i=0; i<mSections.size(); i++)
		{
			if (mSections[i]->getName()==szName)
				return mSections[i];
		}

		if (bCreate)
		{
			mSections.push( CZ_NEW Section());
			mSections.last()->init(szName);
			return mSections.last();
		}

		return NULL;
	}


} // namespace cz

