#include "UnitTest++.h"
#include "czPlatform/czPlatformStableHeaders.h"
#include "czPlatform/czUTF8String.h"
#include "czPlatform/czArray.h"
#include <algorithm>
#include <string>

namespace unitests
{
	// Big string to test the strings growing from quick buffer into heap memory
	const char* gBigString1 = "Really long string, to test big buffer!";
	const char* gBigString2 = "Really long string, to test big buffer!!";
	const char* gBigString3 = "Really long string, to test big buffer!!!";

	template<class StringType>
    void TestStringIterator()
	{
	    using namespace cz;
        using namespace std;
		unsigned char data[] = {'H','e','l','l','o',0};
		//unsigned char reverseddata[] = {'o','l','l','e','H',0};

		// test with empty string
		StringType s1;
		int count=0;
		for (typename StringType::iterator it = s1.begin(); it!= s1.end(); ++it)
		{
			count++;
		}
		CHECK(count==0);

		s1 = (char*)&data[0];
		cz::TArray<u32> res;
		for (typename StringType::iterator it = s1.begin(); it!= s1.end(); it++)
		{
			res.push(*it);
		}

		CHECK_ARRAY_EQUAL(data, res, 5);

		// Test decrement
		typename StringType::iterator it=s1.end();
		u32 cp;
		while(res.pop(cp))
		{
			CHECK_EQUAL(cp, *(--it));
		}

		// Test in an empty string
		StringType s2;
		res.clear();
		for(typename StringType::iterator it=s2.begin(); it!=s2.end(); it++)
		{
			res.push(*it);
		}
		CHECK(res.size()==0);



		// Test std algorithms
		CHECK(std::find(s1.begin(), s1.end(), 'e')!=s1.end());
		it = std::find(s1.begin(), s1.end(), 'A');
		CHECK(it==s1.end());

		it = std::adjacent_find(s1.begin(), s1.end());
		CHECK(it==s1.begin()+2);
		CHECK(std::count(s1.begin(), s1.end(), 'l')==2);
		CHECK(std::count(s1.begin(), s1.end(), 'A')==0);

		CHECK(*std::max_element(s1.begin(), s1.end())=='o');
		CHECK(std::max_element(s2.begin(), s2.end())==s2.end());
		CHECK(*std::min_element(s1.begin(), s1.end())=='H');
		CHECK(std::min_element(s2.begin(), s2.end())==s2.end());
	}

	TEST(TestUTF8String)
	{
		using namespace cz;

		UTF8String stest("Hello");

		UTF8String s;
		CHECK(s.size()==0);
		CHECK(s=="");

		s = "Hello World";
		CHECK(s=="Hello World");
		CHECK(s!="hello World");
		CHECK(s!="Hello world");
		CHECK(s!="Hello worlD");

		s = "B";
		CHECK(s>"A");
		CHECK(s<"C");
		CHECK(s>"AA");
		CHECK(s<"CC");

		// Test non-ansi characters
		//copy1 = "ni\xc3\xb1o";
		UTF8String copy1 = "!\xC2\xA3\xE2\x82\xAC!";
		CHECK(copy1.size()==4);
		CHECK(copy1.sizeBytes()==7); // pound sign takes 2 bytes, and euro sign takes 3, +2 for the enclosing '!'

		//printf("This should display with UTF-8, as a pound and euro sign enclosed in '!' : %s\n", copy1.c_str());

		s = gBigString3;
		CHECK(s==gBigString3);
		s = gBigString2;
		CHECK(s==gBigString2);
		s = gBigString1;
		CHECK(s==gBigString1);
		CHECK(s!=gBigString2);
		CHECK(s!=gBigString3);
		CHECK(s<gBigString2);
		s = copy1;
		copy1 = s;
		CHECK(copy1==s);

		// Test iterator
		TestStringIterator<cz::UTF8String>();
		TestStringIterator<std::string>();

	}

	template<class StringType>
	void TestReverseIterator()
	{
		StringType s("Hello");

		CHECK(std::find(s.rbegin(), s.rend(), 'x')==s.rend());
		CHECK(std::count(s.rbegin(), s.rend(), 'l')==2);

		// Try to find 'o'
		typename StringType::iterator o = std::find(s.begin(), s.end(), 'o');
		typename StringType::reverse_iterator ro = std::find(s.rbegin(), s.rend(), 'o');
		typename StringType::reverse_iterator rh = std::find(s.rbegin(), s.rend(), 'H');
		CHECK_EQUAL( *o, *ro);

		CHECK(*ro=='o');
		CHECK(*rh=='H');

		// Some tests to make sure it behaves the same way as std::string
		CHECK(!(o==ro.base()));
		CHECK(o!=ro.base());
		CHECK(ro==s.rbegin());
		CHECK(ro.base()==s.end());

		CHECK(rh!=s.rend());
		CHECK(!(rh==s.rend()));
		CHECK(rh.base()==s.begin()+1);

		StringType s_reversed;
		for (typename StringType::reverse_iterator i=s.rbegin(); i!=s.rend(); ++i)
		{
			s_reversed += *i;
		}

		CHECK(s=="Hello" && s_reversed=="olleH");

		// Test with empty string
		StringType emptyString;
		CHECK(std::find(emptyString.rbegin(), emptyString.rend(), 'x')==emptyString.rend());

	}

	TEST(TestStringReverseIterator)
	{
		using namespace cz;
		TestReverseIterator<UTF8String>();
		TestReverseIterator<std::string>();
	}


	template<class StringType>
	void _TestStringFindMethods()
	{
		//Positions:  012345678901
		StringType s("Hello World!");
		CHECK_EQUAL( 4, s.find('o', 2) );
		CHECK_EQUAL( 4, s.find('o', 4) );
		CHECK_EQUAL( 7, s.find('o', 5) );
		CHECK_EQUAL( StringType::npos, s.find('x') );
		CHECK_EQUAL( StringType::npos, s.find('o', 100) );

		CHECK_EQUAL( 9, s.find("l", 5) );
		CHECK_EQUAL( 2, s.find("ll", 1) );
		CHECK_EQUAL( StringType::npos, s.find("xx", 0) );
		CHECK_EQUAL( StringType::npos, s.find("ll", 100) );

		CHECK_EQUAL( 9, s.find(StringType("l"), 5) );
		CHECK_EQUAL( 2, s.find(StringType("ll"), 1) );
		CHECK_EQUAL( StringType::npos, s.find(StringType("xx"), 0) );
		CHECK_EQUAL( StringType::npos, s.find(StringType("ll"), 100) );

		// When searching for an empty string (but still with valid offset) it considers it as "found" at
		// the specified offset
		CHECK_EQUAL( 0, s.find(""));
		CHECK_EQUAL( 4, s.find("", 4));
		CHECK_EQUAL( 0, s.find(StringType("")) );
		CHECK_EQUAL( 4, s.find(StringType(""), 4) );

		// In these cases, because the offset is invalid, it returns npos
		CHECK_EQUAL( StringType::npos, s.find("", 100) );
		CHECK_EQUAL( StringType::npos, s.find(StringType(""), 100 ));
	}


	TEST(TestStringFindMethods)
	{
		_TestStringFindMethods<std::string>();
		_TestStringFindMethods<cz::UTF8String>();
	}


	template< class StringType >
	void _TestStringAppend()
	{
		using namespace cz;

		{
			UTF8String s("Hello World!");
			StringType a(std::find(s.begin(), s.end(), 'W'), s.end());
			StringType b(std::find(s.rbegin(), s.rend(), 'W'), s.rend());
			CHECK(a=="World!");
			CHECK(b=="W olleH");
		}

		{
			std::string s("Hello World!");
			StringType a(std::find(s.begin(), s.end(), 'W'), s.end());
			StringType b(std::find(s.rbegin(), s.rend(), 'W'), s.rend());
			CHECK(a=="World!");
			CHECK(b=="W olleH");
		}

	}

	TEST(TestStringAppend)
	{
		_TestStringAppend<std::string>();
		_TestStringAppend<cz::UTF8String>();
	}

	//
	// Test to all STL compatible methods, to make sure they all compile
	//
	template<class StringType>
	void _TestStringAllSTLMethods()
	{
		//
		// Test constructors
		{
			StringType s1;
			CHECK(s1=="");
			s1 = "Hello";
			StringType s2(s1);
			CHECK(s2=="Hello" && s2==s1);
			StringType s3("Hello");
			CHECK(s3=="Hello");


			StringType s4(s3.begin(), s3.end());
			CHECK(s4==s3);
			CHECK(s4=="Hello");

			const std::string stdstring("Hello");
			StringType s5(stdstring.begin(), stdstring.end());
			CHECK(s5==stdstring.c_str());
			CHECK(s5=="Hello");

			// pointer as iterators
			StringType s6(s3.c_str(), s3.c_str()+strlen(s3.c_str()));
			CHECK(s6==s3);
			CHECK(s6=="Hello");
		}

		//
		// Test iterators
		//
		{
			StringType s("Hello");
			CHECK(std::find(s.begin(), s.end(), 'o') == s.begin()+4);
			CHECK(*std::find(s.rbegin(), s.rend(), 'e') == 'e');
		}
		{
			const StringType s("Hello");
			CHECK(std::find(s.begin(), s.end(), 'o') == s.begin()+4);
			CHECK(*std::find(s.rbegin(), s.rend(), 'e') == 'e');
		}

		//
		// Capacity related methods
		//
		{
			StringType s("Hello");
			CHECK(s.size()==strlen("Hello"));
			CHECK(s.length()==strlen("Hello"));
			size_t capacity = s.capacity();
			CHECK(s.max_size()>s.capacity());
			CHECK(static_cast<int>(capacity)>=s.size());
			s = gBigString1;
			CHECK(s.capacity()>static_cast<int>(capacity));
			StringType s1;
			s1.reserve(static_cast<int>(strlen(gBigString1)));
			int oldcapacity = static_cast<int>(s1.capacity());
			s1 = gBigString1;
			CHECK(s1.capacity()==oldcapacity);
			s1.clear();
			CHECK(s1.capacity()==oldcapacity);
			CHECK(s1.empty() && s1=="");
		}

		//
		// Modifiers
		//
		{
			StringType s1;
			s1 = "AB";
			CHECK(s1=="AB");
			s1 = StringType("Hello");
			CHECK(s1=="Hello");

			s1+= " ";
			CHECK(s1=="Hello ");

			s1+= StringType("Wor");
			CHECK(s1=="Hello Wor");

			s1+='l';
			CHECK(s1=="Hello Worl");

			s1.append(StringType("d"));
			CHECK(s1=="Hello World");

			s1.append("!");
			CHECK(s1=="Hello World!");

			s1.append("!");
			CHECK(s1=="Hello World!!");

			StringType s2("ABC");
			s1.append(s2.begin(), s2.end());
			CHECK(s1=="Hello World!!ABC");

			s1.append(s2.rbegin(), s2.rend());
			CHECK(s1=="Hello World!!ABCCBA");

			// Test with pointers as iterators
			s1.append(s2.c_str(), s2.c_str()+s2.size());
			CHECK(s1=="Hello World!!ABCCBAABC");

			std::string s3("123");
			s1.append(s3.begin(), s3.end());
			CHECK(s1=="Hello World!!ABCCBAABC123");

			s1.append(s3.rbegin(), s3.rend());
			CHECK(s1=="Hello World!!ABCCBAABC123321");

		}

		//
		// String operations
		//
		{
			StringType s1("Hello");
			CHECK(s1==s1.c_str() && s1.c_str()==s1.data());

			CHECK(s1.find('o')==4);
			CHECK(s1.find('o',2)==4);
			CHECK(s1.find('o',10)==StringType::npos);
			CHECK(s1.find("ll")==2);
			CHECK(s1.find("ll",2)==2);
			CHECK(s1.find("ll",3)==StringType::npos);
			CHECK(s1.find(StringType("ll"))==2);
			CHECK(s1.find(StringType("ll"),2)==2);
			CHECK(s1.find(StringType("ll"),3)==StringType::npos);

			CHECK(s1.compare(StringType("Hello"))==0);
			CHECK(s1.compare(StringType("hello"))<0); // 'H' < 'h' in the ASCII table
			CHECK(s1.compare(StringType("HEllo"))>0);
			CHECK(s1.compare("Hello")==0);
			CHECK(s1.compare("hello")<0); // 'H' < 'h' in the ASCII table
			CHECK(s1.compare("HEllo")>0);


			s1 = gBigString1;
			s1 += s1;
			std::string big(gBigString1);
			big += gBigString1;
			CHECK(s1==big.c_str());
		}
	}

	TEST(TestStringAllMethods)
	{
		CHECK(strlen(gBigString1)>cz::UTF8String::QUICKBUF_SIZE);
		CHECK(strlen(gBigString2)>cz::UTF8String::QUICKBUF_SIZE);
		CHECK(strlen(gBigString3)>cz::UTF8String::QUICKBUF_SIZE);
		_TestStringAllSTLMethods<std::string>();
		_TestStringAllSTLMethods<cz::UTF8String>();
	}
}
