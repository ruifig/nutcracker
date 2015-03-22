/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com
	
	purpose:
		
*********************************************************************/

#pragma once
#ifndef _czStream_h_
#define _czStream_h_

#include "czPlatformPrerequisites.h"
#include <type_traits>
#include <cstdint>
#include <vector>
#include "czArray.h"
#include "czString.h"

namespace cz
{

	/*! Class writing/reading data to/from a dynamic buffer.
	When writing, memory is allocated as necessary.

	Reading and writing can be done from the same object, but be careful not to read from a stream which
	does not have enough data.
	*/
	class Stream
	{
	public:

		Stream()
		{
			m_externalBuffer.data = nullptr;
			m_externalBuffer.size = 0;
			m_readpos = 0;
			m_readError = false;
		}

		Stream(const void* data, int size)
		{
			m_readpos = 0;
			m_readError = false;
			m_externalBuffer.data = reinterpret_cast<const char*>(data);
			m_externalBuffer.size = size;
		}

		Stream(Stream&& other)
		{
			doMove(other, *this);
		}
		Stream& operator=(Stream&& other)
		{
			doMove(other, *this);
			return *this;
		}

		/*
		Allocates space for writing data, and returns a pointer you can use to write the data on your own
		*/
		void* writeCustom(int size)
		{
			CZ_ASSERT(m_externalBuffer.data==nullptr);
			m_vec.insert(m_vec.end(), size, 0);
			return &m_vec[m_vec.size() - size];
		}

		Stream& write(const void *buf, int size)
		{
			CZ_ASSERT(m_externalBuffer.data==nullptr);
			m_vec.insert(m_vec.end(), static_cast<const char*>(buf), static_cast<const char*>(buf)+size);
			return *this;
		}

		const Stream& read(void *buf, uint32_t size) const
		{
			if (size <= (uint32_t)this->size())
			{
				if (m_externalBuffer.data)
					memcpy(buf, m_externalBuffer.data+m_readpos, size); 
				else
					memcpy(buf, &m_vec[m_readpos], size);
				m_readpos += size;
				return *this;
			}
			else
			{
				m_readError = true;
				return *this;
			}
		}

		const void* readCustom(uint32_t size) const
		{
			if (size==0)
			{
				return nullptr;
			}
			else if (size <= (uint32_t)this->size())
			{
				const void* buf;

				if (m_externalBuffer.data)
					buf =  m_externalBuffer.data + m_readpos;
				else
					buf = &m_vec[m_readpos];

				m_readpos += size;
				return buf;
			}
			else
			{
				m_readError = true;
				return nullptr;
			}
		}

		template<typename T>
		Stream& write(T v)
		{
			static_assert( std::is_arithmetic<T>::value, "Type is not an arithmetic type");
			return write(&v, sizeof(v));
		}

		template<typename T>
		const Stream& read(T& v) const
		{
			static_assert( std::is_arithmetic<T>::value, "Type is not an arithmetic type");
			return read(&v, sizeof(v));
		}

		/*! Returns the total amount of data that's available for reading
		*/
		int size() const
		{
			if (m_externalBuffer.data)
				return m_externalBuffer.size - m_readpos;
			else
				return static_cast<int>(m_vec.size()) - m_readpos;
		}

		/*! return a pointer to the internal memory buffer */
		const void* getPtr() const
		{
			if (m_externalBuffer.data)
			{
				CZ_ASSERT(m_externalBuffer.size);
				return m_externalBuffer.data;
			}
			else
			{
				CZ_ASSERT (m_vec.size());
				return &m_vec[0];
			}
		}

		/*! return a pointer to the internal memory buffer */
		void* getPtr()
		{
			CZ_ASSERT(m_externalBuffer.data==nullptr);
			CZ_ASSERT(m_vec.size());
			return &m_vec[0];
		}

		/*!
			The internal vector used for write operations
			This is only available so we can serialize things, then pass the vector to rpc calls
		*/
		const std::vector<u8>& getWriteVector() const
		{
			return m_vec;
		}

		/*! Overwrite the specified position with a value.
		One use for this, is when writing data for which we don't know the size yet, and then need to go back
		in the stream to update some position which holds that size.
		*/
		template<
			typename T>
			void overwrite(int pos, T v)
		{
			static_assert( std::is_arithmetic<T>::value, "Type is not an arithmetic type");
			CZ_ASSERT(m_externalBuffer.data==nullptr);
			CZ_ASSERT(pos+sizeof(T) <= m_vec.size());
			*reinterpret_cast<T*>(&m_vec[pos]) = v;
		}

		/*!
		Empties the stream.
		*/
		void clear()
		{
			m_externalBuffer.data = nullptr;
			m_externalBuffer.size = 0;
			m_vec.clear();
			m_readpos = 0;
			m_readError = false;
		}

		bool hasReadError() const
		{
			return m_readError;
		}

		/*
		 * Moves the internal vector to the one we specify.
		 * This works like copying the internal vector to the outside, but it uses C++11 move semantics
		 */
		void extractVector(std::vector<u8>& dst);

	private:

		static void doMove(Stream& from, Stream& to)
		{
			to.m_externalBuffer = from.m_externalBuffer;
			to.m_readpos = from.m_readpos;
			to.m_readError = from.m_readError;
			to.m_vec = std::move(from.m_vec);
		}

		// Tells if we're using an external buffer (for reading data from an external source, without copying)
		struct
		{
			const char* data; // NULL if not using an external buffer
			int size; // size of the external buffer in bytes
		} m_externalBuffer;

		std::vector<u8> m_vec;
		mutable int m_readpos;
		mutable bool m_readError;
	};


	template<
		typename T,
	class Enabled=void
	>
	struct VectorSerialization
	{
	private:
		template<typename ContainerType>
		static Stream& _serialize(Stream& stream, const ContainerType& v)
		{
			stream.write<int32_t>(static_cast<int>(v.size()));
			for(const auto& i : v)
			{
				// TODO : If using stream << i, the compiler can't find the operator, but if I call the operator directly with ::operator, it does.
				// No idea why. Maybe something to do with template lookup?
				//::operator<< (stream, i);
				stream << i; // Compiler can't find the operator if I use this
			}
			return stream;
		}
		
		template<typename ContainerType>
		static const Stream& _deserialize(const Stream& stream, ContainerType& v)
		{
			// NOTE:
			// For non arithmetic types, we streamed size is not necessarily sizeof(T)
			// so I can't make checks that.
			uint32_t size;
			stream >> size;
			if (stream.hasReadError())
				return stream;

			v.clear();
			int l=size;
			while(l--)
			{
				T o;
				stream >> o;
				if (stream.hasReadError())
					return stream;
				v.push_back(std::move(o));
			}
			return stream;
		}
	public:

		static Stream& serialize(Stream& stream, const std::vector<T>& v) { return _serialize< std::vector<T> >(stream, v); }
		static const Stream& deserialize(const Stream& stream, std::vector<T>& v) { return _deserialize< std::vector<T> >(stream, v); }
		static Stream& serialize(Stream& stream, const cz::TArray<T>& v) { return _serialize< cz::TArray<T> >(stream, v); }
		static const Stream& deserialize(const Stream& stream, cz::TArray<T>& v) { return _deserialize< cz::TArray<T> >(stream, v); }

	};

	// TODO: Implement vector of bools at some point
	template<>
	struct VectorSerialization<bool, void>
	{
	};

	template< typename T >
	struct VectorSerialization<T, typename std::enable_if< std::is_arithmetic<T>::value >::type>
	{
	private:

		template<typename ContainerType>
		static Stream& _serialize(Stream& stream, const ContainerType& v)
		{
			stream.write<int32_t>(static_cast<int32_t>(v.size()));
			if (v.size())
				return stream.write(&v[0], sizeof(T)*static_cast<int>(v.size()));
			else
				return stream;
		}

		template<typename ContainerType>
		static const Stream& _deserialize(const Stream& stream, ContainerType& v)
		{
			uint32_t size;
			stream >> size;
			if (stream.hasReadError() || (size*sizeof(T)>static_cast<uint32_t>(stream.size())))
				return stream;

			const T* start = reinterpret_cast<const T*>(stream.readCustom(sizeof(T)*size));
			if (stream.hasReadError())
				return stream;
			v.clear();
			v.resize(size);
			if (start && size)
				memcpy(&v[0],  start, sizeof(T)*size);
			return stream;
		}

	public:

		static Stream& serialize(Stream& stream, const std::vector<T>& v) { return _serialize(stream, v); }
		static const Stream& deserialize(const Stream& stream, std::vector<T>& v) { return _deserialize(stream, v); }
		static Stream& serialize(Stream& stream, const cz::TArray<T>& v) { return _serialize(stream, v); }
		static const Stream& deserialize(const Stream& stream, cz::TArray<T>& v) { return _deserialize(stream, v); }
	};

	template< typename T >
	struct VectorSerialization<T, typename std::enable_if< std::is_pointer<T>::value >::type>
	{
		// Serialization not supported for vectors of pointers
		// Trying to serialize a vector will not compile
	};

	inline cz::Stream& operator << (cz::Stream& stream, bool v) { return stream.write(v);  }
	inline cz::Stream& operator << (cz::Stream& stream, uint8_t v) { return stream.write(v);  }
	inline cz::Stream& operator << (cz::Stream& stream, int8_t v) { return stream.write(v);  }
	inline cz::Stream& operator << (cz::Stream& stream, char v) { return stream.write(v);  }
	inline cz::Stream& operator << (cz::Stream& stream, uint16_t v) { return stream.write(v);  }
	inline cz::Stream& operator << (cz::Stream& stream, int16_t v) { return stream.write(v);  }
	inline cz::Stream& operator << (cz::Stream& stream, uint32_t v) { return stream.write(v);  }
	inline cz::Stream& operator << (cz::Stream& stream, int32_t v) { return stream.write(v);  }
	inline cz::Stream& operator << (cz::Stream& stream, uint64_t v) { return stream.write(v);  }
	inline cz::Stream& operator << (cz::Stream& stream, int64_t v) { return stream.write(v);  }
	inline cz::Stream& operator << (cz::Stream& stream, float v) { return stream.write(v);  }
	inline cz::Stream& operator << (cz::Stream& stream, double v) { return stream.write(v);  }

	inline const cz::Stream& operator >> (const cz::Stream& stream, bool& v) { return stream.read(v);  }
	inline const cz::Stream& operator >> (const cz::Stream& stream, uint8_t& v) { return stream.read(v);  }
	inline const cz::Stream& operator >> (const cz::Stream& stream, int8_t& v) { return stream.read(v);  }
	inline const cz::Stream& operator >> (const cz::Stream& stream, char& v) { return stream.read(v);  }
	inline const cz::Stream& operator >> (const cz::Stream& stream, uint16_t& v) { return stream.read(v);  }
	inline const cz::Stream& operator >> (const cz::Stream& stream, int16_t& v) { return stream.read(v);  }
	inline const cz::Stream& operator >> (const cz::Stream& stream, uint32_t& v) { return stream.read(v);  }
	inline const cz::Stream& operator >> (const cz::Stream& stream, int32_t& v) { return stream.read(v);  }
	inline const cz::Stream& operator >> (const cz::Stream& stream, uint64_t& v) { return stream.read(v);  }
	inline const cz::Stream& operator >> (const cz::Stream& stream, int64_t& v) { return stream.read(v);  }
	inline const cz::Stream& operator >> (const cz::Stream& stream, float& v) { return stream.read(v);  }
	inline const cz::Stream& operator >> (const cz::Stream& stream, double& v) { return stream.read(v);  }

	template< typename T >
	inline cz::Stream& operator << (cz::Stream& stream, const std::vector<T>& v)
	{
		return cz::VectorSerialization<T>::serialize(stream, v);
	}

	template< typename T >
	inline const cz::Stream& operator >> (const cz::Stream& stream, std::vector<T>& v)
	{
		return cz::VectorSerialization<T>::deserialize(stream, v);
	}

	template< typename T >
	inline cz::Stream& operator << (cz::Stream& stream, const cz::TArray<T>& v)
	{
		return cz::VectorSerialization<T>::serialize(stream, v);
	}

	template< typename T >
	inline const cz::Stream& operator >> (const cz::Stream& stream, cz::TArray<T>& v)
	{
		return cz::VectorSerialization<T>::deserialize(stream, v);
	}

	template<typename T1, typename T2>
	inline const cz::Stream& operator >> (const cz::Stream& stream, std::pair<T1,T2>& v)
	{
		stream >> v.first;
		stream >> v.second;
		return stream;
	}

	template<typename T1, typename T2>
	inline cz::Stream& operator << (cz::Stream& stream, const std::pair<T1,T2>& v)
	{
		stream << v.first;
		stream << v.second;
		return stream;
	}
	//
	// String serialization
	//
	const cz::Stream& operator >>  (const cz::Stream& stream, std::string& v);
	cz::Stream& operator <<  (cz::Stream& stream, const std::string& v);
	const cz::Stream& operator >>  (const cz::Stream& stream, cz::String& v);
	cz::Stream& operator <<  (cz::Stream& stream, const cz::String& v);
	cz::Stream& operator <<  (cz::Stream& stream, const char* v);

} // namespace cz



#endif // _czStream_h_
