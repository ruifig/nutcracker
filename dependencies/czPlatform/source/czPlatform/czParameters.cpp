/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com
	
	purpose:
	
*********************************************************************/

#include "czPlatformStableHeaders.h"
#include "czParameters.h"

namespace cz
{

cz::UTF8String Parameters::ms_empty;
Parameters::Parameters()
{
}

void Parameters::set(int argc, char* argv[])
{
	if (argc<=1)
		return;
	for (int i=1; i<argc; i++)
	{
		const char *arg = argv[i];
		if (*arg == '-')
			arg++;

		const char *seperator = strchr(arg, '=');
		if (seperator==nullptr)
		{
			m_args.emplace_back(arg, "");
		}
		else
		{
			cz::UTF8String name(arg, seperator);
			cz::UTF8String value(++seperator);
			m_args.emplace_back(std::move(name), std::move(value));
		}
	}
}

const Parameters::Param* Parameters::begin() const
{
	if (m_args.size())
		return &m_args[0];
	else
		return nullptr;
}

const Parameters::Param* Parameters::end() const
{
	return begin() + m_args.size();
}

bool Parameters::has( const char* name ) const
{
	for(auto &i: m_args)
	{
		if (i.name==name)
		{
			return true;
		}
	}
	return false;
}

bool Parameters::has( const cz::UTF8String& name ) const
{
	return has(name.c_str());
}

const cz::UTF8String& Parameters::get( const char *name ) const
{
	for (auto &i: m_args)
	{
		if (i.name==name)
			return i.value;
	}
	return ms_empty;
}

} // namespace cz



