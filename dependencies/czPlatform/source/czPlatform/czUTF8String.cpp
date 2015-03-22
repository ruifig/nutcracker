/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com

	created:	2012/02/12
	created:	12:2:2012   17:31
	filename: 	czUTF8String.cpp
	
	purpose:
	
*********************************************************************/

#include "czPlatformStableHeaders.h"
#include "czUTF8String.h"
#include "../../utfcpp/source/utf8.h"
#include <utility>

namespace cz
{

	//////////////////////////////////////////////////////////////////////////
	// Private/Protected
	//////////////////////////////////////////////////////////////////////////

	const size_t UTF8String::npos = -1;
	const char* UTF8String::msEmptyString = "";

	int UTF8String::_cmp(const char* a, const char* b)
	{
		utf8::uint32_t c1, c2;
		while(*a && *b)
		{
			c1 = utf8::unchecked::next(a);
			c2 = utf8::unchecked::next(b);
			if (c1<c2)
				return -1;
			else if (c1>c2)
				return 1;
		};

		if (*a==*b) // both are 0, so equal
			return 0;
		else if (*a<*b)
			return -1;
		else
			return +1;
	}

	std::pair<int,int> UTF8String::_len(const char* str)
	{
		const char* p = str;
		utf8::uint32_t points = 0;;
		while(*p)
		{
			utf8::unchecked::next(p);
			points++;
		}

		Iterator::difference_type distance = p-str;
		return std::pair<int,int>(static_cast<int>(distance), points);
	}

	std::pair<int,int> UTF8String::_len(const char* start, const char* end)
	{
		Iterator::difference_type distanceCodePoints = utf8::unchecked::distance(start, end);
		Iterator::difference_type distanceBytes = end-start;
		return std::pair<int,int>(static_cast<int>(distanceBytes), static_cast<int>(distanceCodePoints));
	}

	bool UTF8String::checkOverlap(const char* src) const
	{
		return (src>=begin().ptr() && src<=end().ptr());
	}

	const char* UTF8String::getReadPointer() const
	{
		return mData.getReadPointer();
	}

	char* UTF8String::getWritePointer(int writeSizeBytes, bool keepData)
	{
		return mData.getWritePointer(writeSizeBytes, keepData);
	}

	void UTF8String::_appendUtf8(const char* str, int bytes, int codepoints)
	{
		// If the data we want to append is part of our own data, then we need to keep track where is it from,
		// since adding new data can free the current memory, and allocate new one, invalidating the
		// data we want to append.
		const char* doRelocation=0;
		if (checkOverlap(str))
			doRelocation = begin().ptr(); // keep our original begin(), so we can calculate the relocation later

		char* ptr = getWritePointer(sizeBytes()+bytes+1, true) + sizeBytes();
		// Relocate the pointer we want to copy from, if we were copying from our own data
		if (doRelocation)
			str = getReadPointer() + (str-doRelocation);
		memcpy(ptr, str, bytes);
		ptr[bytes] = 0; // Make sure it stays NULL terminated
		mData.updateStringSize(mData.getSizeBytes()+bytes, mData.getSizeCodePoints()+codepoints);
	}

	void UTF8String::_setToUtf8(const char* str, int bytes, int codepoints)
	{
		char* ptr = getWritePointer(bytes+1, false);
		memcpy(ptr, str, bytes);
		ptr[bytes] = 0; // Make sure it stays NULL terminated
		mData.updateStringSize(bytes, codepoints);
	}

	//////////////////////////////////////////////////////////////////////////
	// Constructors and assignment
	//////////////////////////////////////////////////////////////////////////
	
	UTF8String::UTF8String()
	{
	}
	UTF8String::~UTF8String()
	{
	}

	// Copy constructor
	UTF8String::UTF8String(const UTF8String& other) : mData(other.mData)
	{
	}

	UTF8String::UTF8String(const char* str)
	{
		std::pair<int,int> sizes = _len(str);
		_setToUtf8(str, sizes.first, sizes.second);
	}

	UTF8String::UTF8String(UTF8String&& other) : mData(std::move(other.mData))
	{
	}

	UTF8String::UTF8String(const std::wstring& other)
	{
		_setfromwstring(other);
	}

	UTF8String::UTF8String(const wchar_t* other)
	{
		_setfromwstring(other);
	}

	UTF8String::UTF8String(const std::u16string& other)
	{
		_setfromu16string(other);
	}

	UTF8String& UTF8String::operator=(const UTF8String& other)
	{
		mData = other.mData;
		return *this;
	}

	UTF8String& UTF8String::operator=(UTF8String&& other)
	{
		mData = std::move(other.mData);
		return *this;
	}

	UTF8String& UTF8String::operator=(const char *str)
	{
		std::pair<int,int> sizes = _len(str);
		_setToUtf8(str, sizes.first, sizes.second);
		return *this;
	}

	void UTF8String::_setfromwstring(const std::wstring& other)
	{
		std::vector<unsigned char> tmp;
		if (sizeof(wchar_t)==2)
			utf8::unchecked::utf16to8(other.begin(), other.end(), back_inserter(tmp));
		else if (sizeof(wchar_t)==4)
			utf8::unchecked::utf32to8(other.begin(), other.end(), back_inserter(tmp));
		else
		{
			CZ_ASSERT(0);
		}

		tmp.push_back(0);

		_setToUtf8((char*)tmp.data(), (int)tmp.size()-1, _len((char*)tmp.data()).second);
	}


	void UTF8String::_setfromu16string(const std::u16string& other)
	{
		std::vector<unsigned char> tmp;
		utf8::unchecked::utf16to8(other.begin(), other.end(), back_inserter(tmp));
		_setToUtf8((char*)tmp.data(), (int)tmp.size(), _len((char*)tmp.data()).second);
	}

	UTF8String& UTF8String::operator=(const std::wstring& other)
	{
		_setfromwstring(other);
		return *this;
	}


	UTF8String& UTF8String::operator=(const std::u16string& other)
	{
		_setfromu16string(other);
		return *this;
	}

	static u32 g1=0;
	std::u16string UTF8String::toUtf16() const
	{
		std::u16string s;
		utf8::unchecked::utf8to16(getReadPointer(),getReadPointer()+sizeBytes(), back_inserter(s)); 
		return s;
	}

	std::wstring UTF8String::widen() const
	{
		std::wstring s;
		s.reserve(this->size());
		static_assert(sizeof(wchar_t)==2 || sizeof(wchar_t)==4, "Unsupported wchar_t size");
		if (sizeof(wchar_t)==2)
			utf8::unchecked::utf8to16(getReadPointer(), getReadPointer()+sizeBytes(), back_inserter(s));
		else
			utf8::unchecked::utf8to32(getReadPointer(), getReadPointer()+sizeBytes(), back_inserter(s));
		return s;
	}

	//////////////////////////////////////////////////////////////////////////
	// Capacity
	//////////////////////////////////////////////////////////////////////////

	int UTF8String::max_size() const
	{
		#ifdef max
		#undef max
		#endif
		// TODO : Change this if I ever refactor the class to use size_t
		return std::numeric_limits<int>::max();
	}

	void UTF8String::reserve(int numCharacters)
	{
		getWritePointer(numCharacters*4+1, true);
	}

	void UTF8String::clear()
	{
		getWritePointer(0, false)[0] = 0;
		mData.updateStringSize(0, 0);
	}


	/////////////////////////
	// UTF8String::Data
	/////////////////////////

	UTF8String::Data::Data()
	{
		_init();
	}

	UTF8String::Data::Data(const UTF8String::Data& other)
	{
		_init();
		_docopy(other);
	}

	UTF8String::Data::Data(Data&& other)
	{
		_init();
		_domove(std::move(other));
	}

	UTF8String::Data::~Data()
	{
		if (mBuf)
			CZ_FREE(mBuf, MemCat_String);	
	}

	void UTF8String::Data::_init()
	{
		mBuf = nullptr;
		mQuickBuf[0] = 0;
		mStringLengthCodePoints = 0;
		mStringLengthBytes = 0;
		mBufferSize = QUICKBUF_SIZE;
	}

	void UTF8String::Data::_docopy(const UTF8String::Data& other)
	{
		if (this==&other)
			return;
		char* ptr = getWritePointer(other.mStringLengthBytes+1, false);
		mStringLengthCodePoints = other.mStringLengthCodePoints;
		mStringLengthBytes = other.mStringLengthBytes;
		memcpy(ptr, other.getReadPointer(), mStringLengthBytes+1);
	}

	void UTF8String::Data::_domove(UTF8String::Data&& other)
	{
		if (this==&other)
			CZ_UNEXPECTED();

		if (mBuf)
			CZ_FREE(mBuf, MemCat_String);	

		mBuf = other.mBuf;
		mStringLengthCodePoints = other.mStringLengthCodePoints;
		mStringLengthBytes = other.mStringLengthBytes;
		mBufferSize = other.mBufferSize;
		memcpy(mQuickBuf, other.mQuickBuf, sizeof(mQuickBuf));

		// reset other
		other._init();
	}

	UTF8String::Data& UTF8String::Data::operator=(const UTF8String::Data& other)
	{
		_docopy(other);
		return *this;
	}
	UTF8String::Data& UTF8String::Data::operator=(UTF8String::Data&& other)
	{
		_domove(std::move(other));
		return *this;
	}

	const char* UTF8String::Data::getReadPointer() const
	{
		return (mBuf) ? mBuf : mQuickBuf;
	}

	char* UTF8String::Data::getWritePointer(int writeSizeBytes, bool keepData)
	{

		if (writeSizeBytes<=QUICKBUF_SIZE)
		{
			// mBuf size should always be bigger than the quick buffer, otherwise we wouldn't be using it
			return (mBuf) ? mBuf : mQuickBuf;
		}
		else
		{
			if (mBuf && mBufferSize>=writeSizeBytes)
			{
				return mBuf;
			}
			else
			{
				char* newBuf = static_cast<char*>(CZ_ALLOC(writeSizeBytes, MemCat_String));
				if (keepData)
				{
					memcpy(newBuf, (mBuf) ? mBuf : mQuickBuf, mStringLengthBytes+1); // +1 to copy the 0 at the end
				}
				else
				{
					newBuf[0] = 0;
				}

				// Delete current buffer, and set as the new one
				if (mBuf)
					CZ_FREE(mBuf, MemCat_String);
				mBuf = newBuf;
				mBufferSize = writeSizeBytes;
				return mBuf;
			}
		}
	}

	UTF8String::Iterator UTF8String::begin()
	{
		return UTF8String::Iterator(getReadPointer());
	}
	UTF8String::Iterator UTF8String::end()
	{
		return UTF8String::Iterator(getReadPointer()+sizeBytes());
	}

	UTF8String& UTF8String::appendCodepoint(u32 codepoint)
	{
		char buf[5];
		char* start=&buf[0];
		char* end = utf8::unchecked::append(codepoint, start);
		_appendUtf8(start, static_cast<int>(end-buf), 1);
		return *this;
	}

	UTF8String& UTF8String::append(const char* srcStart, const char* srcEnd)
	{
		std::pair<int,int> sizes = _len(srcStart, srcEnd);
		_appendUtf8(srcStart, sizes.first, sizes.second);
		return *this;
	}

	size_t UTF8String::find(u32 codepoint, size_t offset) const
	{
		if (static_cast<int>(offset)>=size())
			return npos;
		iterator it = std::find(begin()+static_cast<int>(offset), end(), codepoint);
		return (it==end()) ? npos : it-begin();
	}

	size_t UTF8String::find(const char* str, size_t offset) const
	{
		if (static_cast<int>(offset)>=size())
			return npos;
		size_t count = strlen(str);
		if (count==0)
			return offset;
		iterator it = std::search(begin()+static_cast<int>(offset), end(), str, str+count);
		return (it==end()) ? npos : it-begin();
	}
	
	size_t UTF8String::find(const UTF8String& str, size_t offset) const
	{
		if (static_cast<int>(offset)>=size())
			return npos;
		if (str.size()==0)
			return offset;
		iterator it = std::search(begin()+static_cast<int>(offset), end(), str.begin(), str.end());
		return (it==end()) ? npos : it-begin();
	}

	// Highly inefficient
	UTF8String& UTF8String::replace(size_t pos, size_t len, const UTF8String& to)
	{
		if (pos>=static_cast<size_t>(size()))
		{
			CZ_UNEXPECTED();
			return *this;
		}

		auto it = begin() + static_cast<int>(pos);
		UTF8String endpart;
		if (pos+len<=static_cast<size_t>(size()))
			endpart.append(it+static_cast<int>(len), end());
		
		*this = UTF8String(begin(), it) + to + endpart;

		return *this;
	}


	//
	// UTF8String iterator
	//
	u32 UTF8String::Iterator::operator * () const
	{
		const char* temp = mIt;
		return utf8::unchecked::next(temp);
	}

	bool UTF8String::Iterator::operator == ( const UTF8String::Iterator& rhs) const
	{
		return (mIt==rhs.mIt);	
	}

	bool UTF8String::Iterator::operator != (const UTF8String::Iterator& rhs) const
	{
		return !(operator == (rhs));
	}

	UTF8String::Iterator& UTF8String::Iterator::operator ++ ()
	{
		utf8::unchecked::next(mIt);
		return *this;
	}
	UTF8String::Iterator UTF8String::Iterator::operator ++ (int) // postfix
	{
		UTF8String::Iterator temp = *this;
		utf8::unchecked::next(mIt);
		return temp;
	}

	UTF8String::Iterator& UTF8String::Iterator::operator -- ()
	{
		utf8::unchecked::previous(mIt);
		return *this;
	}
	UTF8String::Iterator UTF8String::Iterator::operator -- (int) // postfix
	{
		UTF8String::Iterator temp = *this;
		utf8::unchecked::previous(mIt);
		return temp;
	}

	UTF8String::Iterator UTF8String::Iterator::operator- (int codepoints) const
	{
		UTF8String::Iterator temp = *this;
		while(codepoints--)
			utf8::unchecked::previous(temp.mIt);
		return temp;
	}

	UTF8String::Iterator UTF8String::Iterator::operator+ (int codepoints) const
	{
		UTF8String::Iterator temp = *this;
		while(codepoints--)
			utf8::unchecked::next(temp.mIt);
		return temp;
	}

	int UTF8String::Iterator::operator-(const Iterator& other) const
	{
		Iterator::difference_type distance = utf8::unchecked::distance(other.mIt, mIt);
		return static_cast<int>(distance);
	}

	int UTF8String::Iterator::distanceInBytes(const Iterator& start, const Iterator& end)
	{
		Iterator::difference_type distanceBytes = end.mIt - start.mIt;
		return static_cast<int>(distanceBytes);
	}


	UTF8String ansiToLower(const cz::UTF8String& s)
	{
		UTF8String res;
		for(const auto& ch : s)
		{
			if (ch>='A' && ch<='Z')
				res.appendCodepoint( ch + ('a' - 'A') );
			else
				res.appendCodepoint( ch );
		}
		return res;
	}
} // end namespace cz
