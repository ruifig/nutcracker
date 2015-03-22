/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com

	created:	2012/02/21
	created:	21:2:2012   23:14
	filename: 	czFilename.cpp
	
	purpose:
	
*********************************************************************/


#include "czPlatformStableHeaders.h"

#include "czFilename.h"

namespace cz
{


void Filename::set(const UTF8String& path)
{
	UTF8String::operator=(path);
}

namespace
{
	bool isPathSeparator(u32 codepoint)
	{
		return (codepoint=='/' || codepoint=='\\');
	}
	bool notPathSeparator(u32 codepoint)
	{
		return !isPathSeparator(codepoint);
	}
}

cz::UTF8String Filename::removeBackslash() const
{
	UTF8String::const_reverse_iterator it2 = std::find_if(this->rbegin(), this->rend(), isPathSeparator);
	UTF8String::const_reverse_iterator it = std::find_if(this->rbegin(), this->rend(), notPathSeparator);
	if (it==this->rend())
		return *this;
	return UTF8String(this->begin(), it.base());
}

UTF8String Filename::getFilename() const
{
	UTF8String::const_reverse_iterator it = std::find_if(this->rbegin(), this->rend(), isPathSeparator);
	if (it==this->rend())
		return *this;
	return UTF8String(it.base(), this->end());
}

UTF8String Filename::getBaseFilename() const
{
	const UTF8String fname = getFilename();
	UTF8String::const_reverse_iterator it = std::find(fname.rbegin(), fname.rend(), '.');
	if (it!=fname.rend())
		return UTF8String(fname.begin(), it.base()-1);
	else
		return UTF8String("");
}

UTF8String Filename::getDirectory() const
{
	UTF8String::const_reverse_iterator it = std::find_if(this->rbegin(), this->rend(), isPathSeparator);
	if (it!=this->rend())
		return UTF8String(this->begin(), it.base());
	else
		return UTF8String("");
}


UTF8String Filename::getExtension(bool includeDot) const
{
	const UTF8String fname = getFilename();
	UTF8String::const_reverse_iterator it = std::find(fname.rbegin(), fname.rend(), '.');
	if (it!=fname.rend())
		return UTF8String(it.base() - ((includeDot) ? 1 : 0), fname.end());
	else
		return UTF8String("");
}

const UTF8String& Filename::getFullPath() const
{
	return *this;
}

bool Filename::isRelativePath() const
{
	if (this->empty())
		return true;
	UTF8String::const_iterator it = this->begin();
	if (isPathSeparator(*it))
		return false; // absolute path
	else if (*it=='.')
		return true; // relative path

	*it++;
	if (it==this->end())
		return true; // relative path. It's a single letter filename (e.g: "f")
	else if (*it==':')
		return false; // absolute path. It starts with a drive letter (e.g: "C:\xxx")
	else
		return true; // relative path
}


} // namespace cz
