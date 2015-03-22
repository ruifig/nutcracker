/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com
	
	purpose:
	
*********************************************************************/

#pragma once

#include "czPlatformPrerequisites.h"
#include "czUTF8String.h"

namespace cz
{

class Parameters
{
public:
	struct Param
	{
		template<class T1, class T2>
		Param(T1&& name_, T2&& value_) : name(std::forward<T1>(name_)), value(std::forward<T2>(value_)){}
		cz::UTF8String name;
		cz::UTF8String value;
	};
	Parameters();
	void set(int argc, char* argv[]);
	const Param* begin() const;
	const Param* end() const;
	bool has(const char* name) const;
	bool has(const UTF8String& name) const;
	const cz::UTF8String& get(const char *name) const;
	void clear()
	{
		m_args.clear();
	}
private:
	static cz::UTF8String ms_empty;
	std::vector<Param> m_args;
};

} // namespace cz

