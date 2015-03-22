/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com
	
	purpose:
	Type that behaves like a variable that can hold several different types
	
*********************************************************************/

#pragma once
#ifndef _czAny_h_
#define _czAny_h_

#include "czPlatformPrerequisites.h"
#include <vector>
#include <stdint.h>
#include <czPlatform/czString.h>
#include <future>

namespace cz
{

	class Stream;

	/*
	Used to hold parameters passed for server console commands
	*/
	class clcpp_attr(reflect_part) Any
	{
	public:
		Any();
		Any(const Any&) = default;
		Any(Any&& other);
		explicit Any(bool v);
		explicit Any(int v);
		explicit Any(unsigned int v);
		explicit Any(float v);
		explicit Any(const char* v);
		explicit Any(const String& v);
		explicit Any(String&& v);
		explicit Any(std::vector<uint8_t>&& blob);
		explicit Any(const std::vector<uint8_t>& blob);

		//static optional<Any> create(const String& str);

		enum Type
		{
			kNone,
			kBool, // bool is encoded as an integer 0 or 1
			kInteger,
			kUnsignedInteger,
			kFloat,
			kString,
			kBlob
		};

		const cz::Stream& readFromStream(const cz::Stream& stream);
		cz::Stream& saveToStream(cz::Stream& stream) const;
		Type getType() const
		{
			return m_type;
		}

		bool asBool(bool &v) const;
		bool asInteger(int& v) const;
		bool asUnsignedInteger(unsigned int& v) const;
		bool asString(String& v) const;
		bool asFloat(float& v) const;
		bool asBlob(std::vector<uint8_t>& v) const;

		const char* convertToString() const;

		Any& operator= (const Any& other);
		Any& operator= (Any&& other);

		template<class T>
		Any& operator= (T&& v)
		{
			set( Any(std::forward<T>(v)) );
			return *this;
		}

	private:

		void set(const Any& other);
		void set(Any&& other);

		Type m_type;
		union
		{
			int m_integer;
			unsigned int m_uinteger;
			float m_float;
		};
		String m_string;
		std::vector<uint8_t> m_blob;
	};


	const cz::Stream& operator >> (const cz::Stream& stream, cz::Any &v);
	cz::Stream& operator << (cz::Stream& stream, const cz::Any &v);

	struct clcpp_attr(reflect) AnyTree
	{
		AnyTree()
		{
		}
		AnyTree(std::string name_, Any data_)
		: name(name_), data(data_)
		{
		}

		AnyTree(AnyTree&& other)
		: name(std::move(other.name))
		, data(std::move(other.data))
		, children(std::move(other.children))
		{
		}

		AnyTree& operator=(AnyTree&& other)
		{
			name = std::move(other.name);
			data = std::move(other.data);
			children = std::move(other.children);
			return *this;
		}

		AnyTree(const AnyTree&) = default;
		AnyTree& operator=(const AnyTree&) = default;

		std::string name;
		Any data;
		std::vector<AnyTree> children;
	};

	const cz::Stream& operator >> (const cz::Stream& stream, cz::AnyTree &v);
	cz::Stream& operator << (cz::Stream& stream, const cz::AnyTree &v);

} // namespace cz


//
// Forced template instantiations, otherwise clReflect won't find them
//
template class std::vector<cz::AnyTree>;
template class std::future<std::vector<cz::AnyTree>>;

#endif // _czAny_h_
