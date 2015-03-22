/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com

	created:	2012/02/12
	created:	12:2:2012   17:29
	filename: 	czUTF8String.h

	purpose:
	UTF8 String class
*********************************************************************/

#ifndef czUTF8String_h__
#define czUTF8String_h__

#include "czPlatformPrerequisites.h"
#include <iterator>
#include <utility>
#include <ostream>

namespace cz
{


/*! \addtogroup String handling
	@{
*/

	/*! UTF8 string class.
	It's not entirely compatible with std::string, but it does implement some of the same methods.
	In most cases, it should be compatible.

	\note
	Note that because UTF-8 is a multibyte stream, calculating a character's position in a string is not as simple
	as using an array index, like for C-style strings. Retrieving a character at an arbitrary position requires
	processing the string from the beginning.
	For this reason, some std::string equivalent methods are missing, or are not as fast as std::string.
	Also, that's the reason why the iterator dereferencing operator ( UTF8String::Iterator::operator*() ) returns
	a value, and not a reference.

	In most cases, you're better of using methods that deal with iterators, instead of character positions.
	*/
	class UTF8String : public TAllocatedObject<StringCategoryAllocator>
	{
	public:

		enum
		{
			/*! Size of the internal quick buffer, used for small strings */
			QUICKBUF_SIZE = 32
		};


		//////////////////////////////////////////////////////////////////////////
		// Iterator class
		//////////////////////////////////////////////////////////////////////////

		/*! String iterator.
		Can be used with STL algorithms.
		 */
		class Iterator : public std::iterator<std::bidirectional_iterator_tag, u32, std::ptrdiff_t, u32*, u32>
		{
		public:
			Iterator() : mIt(NULL)
			{
			}
			Iterator(const char* it) : mIt(it)
			{
			}

			Iterator(const Iterator& other) : mIt(other.mIt) {}
			Iterator& operator=(const Iterator& other)
			{
				mIt = other.mIt;
				return *this;
			}

			u32 operator * () const;
			bool operator == (const Iterator& rhs) const;
			bool operator != (const Iterator& rhss) const;
			Iterator& operator ++ ();
			Iterator operator ++ (int); // postfix
			Iterator& operator -- ();
			Iterator operator -- (int); // postfix
			Iterator operator-(int) const;
			Iterator operator+(int) const;

			bool operator<(const Iterator& other) const
			{
				return mIt<other.mIt;
			}
			bool operator<=(const Iterator& other) const
			{
				return mIt<=other.mIt;
			}
			bool operator>(const Iterator& other) const
			{
				return mIt>other.mIt;
			}
			bool operator>=(const Iterator& other) const
			{
				return mIt>=other.mIt;
			}

			// Used to calculate distances between iterators, in codepoints (NOT bytes)
			int operator-(const Iterator& other) const;
			const char* ptr() const
			{
				return mIt;
			}

			static int distanceInBytes(const Iterator& start, const Iterator& end);

		private:
			const char* mIt;
		};

		/*! To make it compatible with std::string */
		typedef Iterator iterator;
		/*! To make it compatible with std::string */
		typedef Iterator const_iterator;
		/*! To make it compatible with std::string */
		typedef std::reverse_iterator<Iterator> reverse_iterator;
		/*! To make it compatible with std::string */
		typedef reverse_iterator const_reverse_iterator;


		/*! \name STL compatible members
			@{
		*/

		/*! Value used in some methods to indicate "out of range" or "as many as possible".
		*/
		static const size_t npos;

		//////////////////////////////////////////////////////////////////////////
		// Constructors
		//////////////////////////////////////////////////////////////////////////

		UTF8String();
		~UTF8String();

		// Copy constructor
		UTF8String(const UTF8String& other);
		/*!
		\param str utf-8 string
		*/
		UTF8String(const char* str);
		// Move constructor
		UTF8String(UTF8String&& other);

		/*! Constructs the string from iterators */
		template<class SomeIterator>
		UTF8String(SomeIterator srcStart, SomeIterator srcEnd)
		{
			append(srcStart, srcEnd);
		}

		/*
		\param other
			Treated as utf-16 on platforms where sizeof(wchar_t) is 2 or utf-32 on
			platforms where sizeof(wchar_t) is 4
		*/
		UTF8String(const std::wstring& other);
		UTF8String(const wchar_t* other);


		UTF8String(const std::u16string& other);

		/*! Constructs the string from pointers (behaves the same as iterators)
		Input is treated as utf8
		*/
		UTF8String(const char* srcStart, const char* srcEnd)
		{
			append(srcStart, srcEnd);
		}

		//////////////////////////////////////////////////////////////////////////
		// Iterators
		//////////////////////////////////////////////////////////////////////////

		/*! Returns an iterator to the first element */
		iterator begin();
		/*! Returns an iterator to the first element */
		const_iterator begin() const
		{
			return (const_cast<UTF8String*>(this))->begin();
		}

		/*! Returns an iterator that addresses the location succeeding the last element in the string */
		iterator end();
		/*! Returns an iterator that addresses the location succeeding the last element in the string */
		const_iterator end() const
		{
			return (const_cast<UTF8String*>(this))->end();
		}

		reverse_iterator rbegin()
		{
			return UTF8String::reverse_iterator(end());
		}

		const_reverse_iterator rbegin() const
		{
			return UTF8String::reverse_iterator(end());
		}

		reverse_iterator rend()
		{
			return UTF8String::reverse_iterator(begin());
		}
		const_reverse_iterator rend() const
		{
			return UTF8String::reverse_iterator(begin());
		}


		//////////////////////////////////////////////////////////////////////////
		// Capacity
		//////////////////////////////////////////////////////////////////////////

		//! Returns the string length in code points (not bytes)
		int size() const
		{
			return mData.getSizeCodePoints();
		}

		//! Same as #size
		int length() const
		{
			return size();
		}

		//! Returns the maximum size in characters this string class can handle
		int max_size() const; // Putting this in the CPP file, so I can #undef Windows max/min macros safely

		/*! Returns how many characters the string can hold without allocating more memory*/
		int capacity() const
		{
			return mData.getCapacityCodepoints();
		}

		/*! Sets the capacity of the string to at least the specified number of characters.
		\param numCharacters
			Minimum number characters to set the capacity to.
			Since a character in UTF8 format can take up to 4 bytes, the amount of memory
			to guarantee the requested capacity is numCharacters*4, which is what it will be allocated
			internally. Be aware that it means this method allocates 4 times more memory than a call
			to the equivalent std::string::reserve method.
		*/
		void reserve(int numCharacters);

		/*! Sets the string to empty */
		void clear();

		/*! Tells if the string is empty */
		bool empty() const
		{
			return (size()==0);
		}


		//////////////////////////////////////////////////////////////////////////
		// Modifiers
		//////////////////////////////////////////////////////////////////////////

		UTF8String& operator=(const UTF8String& str);
		UTF8String& operator=(UTF8String&& str);
		UTF8String& operator=(const char *str);

		UTF8String& operator+= (const UTF8String& str)
		{
			return append(str);
		}

		/*
		\param other
			Treated as utf-16 on platforms where sizeof(wchar_t) is 2 or utf-32 on
			platforms where sizeof(wchar_t) is 4
		*/
		UTF8String& operator=(const std::wstring& other);

		UTF8String& operator=(const std::u16string& other);

		UTF8String& operator+= (const char* str)
		{
			return append(str);
		}

		UTF8String& operator+= (u32 codepoint)
		{
			return appendCodepoint(codepoint);
		}

		/*! Appends another string */
		UTF8String& append(const UTF8String& str)
		{
			// TODO test this when doing something like a.append(a.a())
			return append(str.begin().ptr(), str.end().ptr());
		}

		UTF8String& append(const char* str)
		{
			// TODO test this when doing something like a.append(a.c_str())
			return append(str, str+strlen(str));
		}

		/*! Appends data from another string
		\param srcStart Iterator to the first character to get
		\param srcEnd Iterator to the position one past the last character to get
		\return Returns *this
		*/
		UTF8String& append(Iterator srcStart, Iterator srcEnd)
		{
			return append(srcStart.ptr(), srcEnd.ptr());
		}

		template<class SomeIterator>
		UTF8String& append(SomeIterator srcStart, SomeIterator srcEnd)
		{
			while(srcStart!=srcEnd)
			{
				operator+= (*srcStart);
				++srcStart;
			}
			return *this;
		}

		UTF8String& append(const char* srcStart, const char* srcEnd);

		//////////////////////////////////////////////////////////////////////////
		// Operators
		//////////////////////////////////////////////////////////////////////////

		/*! Returns the string as a C style string
		For performance reasons the returned pointer is the internal utf8 string, and not the string converted to ansi.
		*/
		const char* c_str() const
		{
			return getReadPointer();
		}

		/*! Same as #c_str */
		const char* data() const
		{
			return c_str();
		}

		/*! Search for the specified character (specified with an unicode codepoint).
		\param codepoint character to search for
		\param offset Character index to at which to begin the search.
		\return
			The index at which the character was found, or #npos if not found. Note that this is not an index to be used
			with a c-style array, since utf8 strings are multibyte strings.
			If you intend to use the return value to access the string again somehow, it's recommended
			to use iterators instead, like for example with std::find algorithm
		*/
		size_t find(u32 codepoint, size_t offset = 0) const;
		/*! Searches for a string (encoded as ANSI ) and returns where it was found, or #npos if not.
		*/
		size_t find(const char* str, size_t offset = 0) const;
		/*! Searches for a string and returns where it was found, or #npos if not.*/
		size_t find(const UTF8String& str, size_t offset = 0) const;

		/* Kind of the same thing as std::string::replace, but highly inneficient */
		UTF8String& replace(size_t pos, size_t len, const UTF8String& to);

		/*! Compares to another string.
		\return returns 0 if strings are equal, a negative value if "this < str", or positive value if "this > str"
		*/
		int compare(const UTF8String& str) const
		{
			if (this==&str)
				return 0;
			else
				return _cmp(getReadPointer(), str.getReadPointer());
		}

		int compare(const char* str) const
		{
			return _cmp(getReadPointer(), str);
		}


		//////////////////////////////////////////////////////////////////////////
		// Conversion
		//////////////////////////////////////////////////////////////////////////
		std::u16string toUtf16() const;
		std::wstring widen() const;


		//////////////////////////////////////////////////////////////////////////
		// Non-stl methods
		//////////////////////////////////////////////////////////////////////////

		UTF8String& appendCodepoint(u32 codepoint);

		/*! returns the size of the string, in bytes */
		int sizeBytes() const
		{
			return mData.getSizeBytes();
		}

		/*! Tells if it's using the same internal data
		*/
		bool _same(const UTF8String& other) const
		{
			return this==&other;
		}


	private:

		const char* getReadPointer() const;
		char* getWritePointer(int writeSizeBytes, bool keepData);

		bool checkOverlap(const char* src) const;

		// Compares two UTF-8 strings
		// returns:
		// <0 : if a<b
		//  0  : if a==b
		// >0 : if a>b
		static int _cmp(const char* a, const char* b);

		// Calculates the length in <bytes,codepoints> of the specified string, not counting the terminating 0
		static std::pair<int,int> _len(const char* str);
		// Calculates the length in <bytes,codepoints> between the two specified pointers
		static std::pair<int,int> _len(const char* start, const char* end);

		// Appends valid utf8 data
		void _appendUtf8(const char* str, int bytes, int codepoints);

		// Sets the string to the specified utf8 data
		void _setToUtf8(const char* str, int bytes, int codepoints);

		void _setfromwstring(const std::wstring& other);
		void _setfromu16string(const std::u16string& other);

		class Data : public TAllocatedObject<StringCategoryAllocator>
		{
		public:
			Data();
			~Data();
			Data(const Data& other);
			Data(Data&& other);
			Data& operator=(const Data& other);
			Data& operator=(Data&& other);

			void updateStringSize(int lengthBytes, int lengthCodePoints)
			{
				mStringLengthBytes = lengthBytes;
				mStringLengthCodePoints = lengthCodePoints;
			}
			int getSizeCodePoints() const
			{
				return mStringLengthCodePoints;
			}
			int getSizeBytes() const
			{
				return mStringLengthBytes;
			}

			int getCapacityBytes() const
			{
				return mBufferSize-1;
			}

			int getCapacityCodepoints() const
			{
				// Consider that a character can take up to 4 bytes
				return getCapacityBytes() / 4;
			}

		protected:
			const char* getReadPointer() const;
			char* getWritePointer(int writeSizeBytes, bool keepData);
			// Only allow access to internal data with these function, to enforce Copy-On-Write
			friend const char* UTF8String::getReadPointer() const;
			friend char* UTF8String::getWritePointer(int writeSizeBytes, bool keepData);
		private:
			void _init();
			void _docopy(const Data& other);
			void _domove(Data&& other);
			// Private to avoid use
			char* mBuf; // allocated buffer. If it's NULL, it means we're using the quickBuf
			int mStringLengthCodePoints; // length of the string, in code points (NOT BYTES), excluding the 0 at the end
			int mStringLengthBytes; // length of the string, in bytes, excluding the 0 at the end
			int mBufferSize; // size of the allocated buffer, in bytes
			char mQuickBuf[QUICKBUF_SIZE];
		};

		Data mData;
		static const char* msEmptyString;

	};

	template <class traits>
	std::basic_ostream<char,traits >& operator<< (std::basic_ostream<char,traits>& os, const UTF8String& str )
	{
		os << str.c_str();
		return os;
	}


	//////////////////////////////////////////////////////////////////////////
	// Comparison/Other operators
	//////////////////////////////////////////////////////////////////////////

	inline bool operator==(const UTF8String& left, const UTF8String& right)
	{
		return (left.compare(right)==0);
	}

	inline bool operator==(const UTF8String& left, const char* right)
	{
		return (left.compare(right)==0);
	}

	inline bool operator==(const char* left, const UTF8String& right)
	{
		return (right.compare(left)==0);
	}

	inline bool operator!=(const UTF8String& left, const UTF8String& right)
	{
		if (left._same(right))
			return false;
		else
			return (left.compare(right)!=0);
	}

	inline bool operator!=(const UTF8String& left, const char* right)
	{
		return (left.compare(right)!=0);
	}

	inline bool operator!=(const char* left, const UTF8String& right)
	{
		return (right.compare(left)!=0);
	}

	inline bool operator<(const UTF8String& left, const UTF8String& right)
	{
		if (left._same(right))
			return false;
		else
			return (left.compare(right)<0);
	}

	inline bool operator<(const UTF8String& left, const char* right)
	{
		return (left.compare(right)<0);
	}

	inline bool operator<(const char* left, const UTF8String& right)
	{
		return (right.compare(left)>0);
	}

	inline bool operator>(const UTF8String& left, const UTF8String& right)
	{
		if (left._same(right))
			return false;
		else
			return (left.compare(right)>0);
	}

	inline bool operator>(const UTF8String& left, const char* right)
	{
		return (left.compare(right)>0);
	}

	inline bool operator>(const char* left, const UTF8String& right)
	{
		return (right.compare(left)<0);
	}

	inline bool operator >= (const UTF8String& left, const UTF8String& right)
	{
		if (left._same(right))
			return true;
		else
			return (left.compare(right)>=0);
	}

	inline bool operator >= (const UTF8String& left, const char* right)
	{
		return (left.compare(right)>=0);
	}

	inline bool operator >= (const char* left, const UTF8String& right)
	{
		return (right.compare(left)<=0);
	}

	inline bool operator <= (const UTF8String& left, const UTF8String& right)
	{
		if (left._same(right))
			return true;
		else
			return (left.compare(right)<=0);
	}

	inline bool operator <= (const UTF8String& left, const char* right)
	{
		return (left.compare(right)<=0);
	}

	inline bool operator <= (const char* left, const UTF8String& right)
	{
		return (right.compare(left)>=0);
	}


	inline UTF8String operator + (const UTF8String& left, const UTF8String& right)
	{
		UTF8String tmp;
		tmp.append(left);
		tmp.append(right);
		return tmp;
	}

	inline UTF8String operator + (const UTF8String& left, const char* right)
	{
		UTF8String tmp;
		tmp.append(left);
		tmp.append(right);
		return tmp;
	}

	/**
	* Converts a string to lower case, but _ONLY_ the ansi characters.
	* Any non ansi characters are left unchanged
	*/
	UTF8String ansiToLower(const cz::UTF8String& s);

/*!
	@}
*/

} // end namespace cz



#endif // czUTF8String_h__
