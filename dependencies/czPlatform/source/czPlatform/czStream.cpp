/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com
	
	purpose:
	
*********************************************************************/

#include "czPlatformStableHeaders.h"
#include "czStream.h"

//
// String serialization
//
namespace
{
	template<class T>
	const cz::Stream&  _deserializeString (const cz::Stream& stream, T& v)
	{
		uint32_t len;
		stream >> len;
		if (!stream.hasReadError())
		{
			const char* ptr = reinterpret_cast<const char*>(stream.readCustom(len));
			if (!stream.hasReadError())
			{
				v.clear();
				v.reserve(len);
				v.insert(v.begin(), ptr, ptr+len);
			}
		}
		return stream;
	}

	template<class T>
	cz::Stream& _serializeString (cz::Stream& stream, const T& v)
	{
		stream << int32_t(v.size());
		stream.write(v.c_str(), static_cast<int>(v.size()));
		return stream;
	}

}

namespace cz
{

	const cz::Stream& operator >>  (const cz::Stream& stream, cz::String& v)
	{
		return _deserializeString(stream, v);
	}
	const cz::Stream& operator >>  (const cz::Stream& stream, std::string& v)
	{
		return _deserializeString(stream, v);
	}

	cz::Stream& operator <<  (cz::Stream& stream, const cz::String& v)
	{
		return _serializeString(stream, v);
	}

	cz::Stream& operator <<  (cz::Stream& stream, const std::string& v)
	{
		return _serializeString(stream, v);
	}

	cz::Stream& operator <<  (cz::Stream& stream, const char* v)
	{
		int32_t len = static_cast<int32_t>(strlen(v));
		stream << len;
		stream.write(v, len);
		return stream;
	}


	void Stream::extractVector(std::vector<u8>& dst)
	{
		dst = std::move(m_vec);
		clear();
	}

} // namespace cz


