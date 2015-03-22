/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com
	
	purpose:
	Type that behaves like a variable that can hold several different types
	
*********************************************************************/

#include "czPlatformStableHeaders.h"
#include "czAny.h"
#include "czStream.h"
#include "czLogger.h"

namespace cz
{

Any::Any()
{
	static_assert( sizeof(m_integer)==sizeof(m_float), "Check the union" );
	m_type = kNone;
}

Any::Any(Any&& other) :
	m_type( std::move(other.m_type) ),
	m_integer( std::move( other.m_integer) ),
	m_string( std::move(other.m_string) ),
	m_blob( std::move(other.m_blob) )
{
}

Any::Any(bool v) : m_type(kBool), m_integer(v ? 1 : 0)
{
}
Any::Any(int v) : m_type(kInteger), m_integer(v)
{
}
Any::Any(unsigned int v) : m_type(kUnsignedInteger), m_uinteger(v)
{
}
Any::Any(float v) : m_type(kFloat), m_float(v)
{
}
Any::Any(const char* v) : m_type(kString), m_string(v)
{
}
Any::Any(const String& v) : m_type(kString), m_string(v)
{
}
Any::Any(String&& v) : m_type(kString), m_string(std::move(v))
{
}

Any::Any(std::vector<uint8_t>&& blob)
	: m_type(kBlob), m_blob(std::move(blob))
{
}

Any::Any(const std::vector<uint8_t>& blob)
	: m_type(kBlob), m_blob(blob)
{
}

const cz::Stream& Any::readFromStream(const cz::Stream& stream)
{
	uint8_t t = kNone;
	stream >> t;
	m_type = static_cast<Type>(t);
	switch(m_type)
	{
	case kNone:
		break;
	case kString:
		stream >> m_string;
		break;
	case kBool:
		{
			uint8_t v;
			stream >> v;
			m_integer = v ? 1 : 0;
		}
		break;
	case kInteger:
	case kUnsignedInteger:
		stream >> m_integer;
		break;
	case kFloat:
		stream >> m_float;
		break;
	case kBlob:
		stream >> m_blob;
		break;
	default:
		czError(ID_Log, "Invalid Generic parameter type (%d)", m_type);
	}
	return stream;
}

cz::Stream& Any::saveToStream(cz::Stream& stream) const
{
	stream << uint8_t(m_type);
	switch(m_type)
	{
	case kNone:
		break;
	case kString:
		stream << m_string;
		break;
	case kBool:
		{
			uint8_t v = m_integer ? 1 : 0;
			stream << v;
		}
		break;
	case kInteger:
	case kUnsignedInteger:
		stream << m_integer;
		break;
	case kFloat:
		stream << m_float;
		break;
	case kBlob:
		stream << m_blob;
		break;
	default:
		CZ_UNEXPECTED();
	}
	return stream;
}

bool Any::asBool(bool & v) const
{
	if (m_type==kBool || m_type==kInteger || m_type==kUnsignedInteger)
	{
		v = m_integer!=0;
		return true;
	}
	else
	{
		return false;
	}
}

bool Any::asInteger(int& v) const
{
	if (m_type==kInteger)
	{
		v = m_integer;
		return true;
	}
	else if (m_type==kFloat)
	{
		v = static_cast<int>(m_float);
		return true;
	}
	else
	{
		return false;
	}
}

bool Any::asUnsignedInteger(unsigned int& v) const
{
	if (m_type==kUnsignedInteger)
	{
		v = m_integer;
		return true;
	}
	else if (m_type==kFloat)
	{
		v = static_cast<unsigned int>(m_float);
		return true;
	}
	else
	{
		return false;
	}
}

bool Any::asString(String& v) const
{
	if (m_type==kString)
	{
		v = m_string;
		return true;
	}
	else
	{
		return false;
	}
}
bool Any::asFloat(float& v) const
{
	if (m_type==kFloat)
	{
		v = m_float;
		return true;
	}
	else if (m_type==kInteger)
	{
		v = static_cast<float>(m_integer);
		return true;
	}
	else if (m_type==kUnsignedInteger)
	{
		v = static_cast<float>(m_uinteger);
		return true;
	}
	else
	{
		return false;
	}
}

bool Any::asBlob(std::vector<uint8_t>& v) const
{
	if (m_type==kBlob)
	{
		v = m_blob;
		return true;
	}
	else
	{
		return false;
	}
}


const char* Any::convertToString() const
{
	switch(m_type)
	{
		case kNone:
			return "";
		case kBool:
			return m_integer ? "true" : "false";
		case kInteger:
			return cz::formatString("%d", m_integer);
		case kUnsignedInteger:
			return cz::formatString("%u", m_integer);
		case kFloat:
			return cz::formatString("%.4f", m_float);
		case kString:
			return m_string.c_str();
		case kBlob:
			return "BINARY BLOB";
		default:
			CZ_UNEXPECTED();
			return "";
	}
}

void Any::set(const Any& other)
{
	m_type = other.m_type;
	m_integer = other.m_integer;
	m_string = other.m_string;
	m_blob = other.m_blob;
}
void Any::set(Any&& other)
{
	m_type = std::move(other.m_type);
	m_integer = std::move(other.m_integer);
	m_string = std::move(other.m_string);
	m_blob = std::move(other.m_blob);
}

Any& Any::operator=(const Any& other)
{
	set(other);
	return *this;
}

Any& Any::operator=(Any&& other)
{
	set(std::move(other));
	return *this;
}


const cz::Stream& operator >> (const cz::Stream& stream, cz::Any &v)
{
	return v.readFromStream(stream);
}
cz::Stream& operator << (cz::Stream& stream, const cz::Any &v)
{
	return v.saveToStream(stream);
}

const cz::Stream& operator >> (const cz::Stream& stream, cz::AnyTree &v)
{
	stream >> v.name;
	stream >> v.data;
	stream >> v.children;
	return stream;
}

cz::Stream& operator << (cz::Stream& stream, const cz::AnyTree &v)
{
	stream << v.name;
	stream << v.data;
	stream << v.children;
	return stream;
}

} // namespace cz
