/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com
	
	purpose:
	
*********************************************************************/

#include "czPlatformStableHeaders.h"

#include "czRingBuffer.h"

namespace cz
{

	RingBuffer::RingBuffer()
	{
		m_pBuf = 0;
		m_fillcount = 0;
		m_readpos = 0;
		m_writepos = 0;
		m_maxsize = 0;
	}

	RingBuffer::~RingBuffer()
	{
		if (m_pBuf)
		{
			CZ_FREE(m_pBuf, MemCat_General);
		}
	}

	void RingBuffer::reserve(int size)
	{
		if (m_maxsize>=size)
			return;

		char *newbuf = (char*) CZ_ALLOC(size, MemCat_General);
		if (m_pBuf)
		{
			//! Copy existing data to the new buffer
			if (m_fillcount)
			{
				int len = std::min(m_fillcount, m_maxsize-m_readpos);
				memcpy(&newbuf[0], &m_pBuf[m_readpos], len);
				if (len!=m_fillcount)
				{
					memcpy(&newbuf[len], &m_pBuf[0], m_fillcount-len);
				}

			}

			CZ_FREE(m_pBuf, MemCat_General);
		}

		m_pBuf = newbuf;
		m_readpos = 0;
		m_writepos = m_fillcount;
		m_maxsize = size;
	}

	void RingBuffer::clear(bool releaseMemory)
	{
		m_fillcount = 0;
		m_readpos = 0;
		m_writepos = 0;
		if (releaseMemory && m_pBuf)
		{
			CZ_FREE(m_pBuf, MemCat_General);
			m_pBuf = nullptr;
			m_maxsize = 0;
		}
	}

	int RingBuffer::write(const void *ptr, int size)
	{
		void *p1, *p2;
		int s1, s2;
		int writeSize = write(size, &p1, &s1, &p2, &s2);

		memcpy(p1, ptr, s1);
		if (p2)
			memcpy(p2, ((u8*)ptr)+s1, s2);

		return size;
	}

	int RingBuffer::write(int writeSize, void** ptr1, int* ptr1size, void **ptr2, int* ptr2size)
	{
		if (getFreeSize() < writeSize)
			reserve(m_maxsize + writeSize - getFreeSize());

		if (m_maxsize-m_writepos >= writeSize) // In case we don't need to wrap around
		{
			*ptr1 = &m_pBuf[m_writepos];
			*ptr1size = writeSize;
			*ptr2 = NULL;
			*ptr2size = 0;
		}
		else
		{
			*ptr1 = &m_pBuf[m_writepos];
			*ptr1size = m_maxsize-m_writepos;
			*ptr2 = &m_pBuf[0];
			*ptr2size = writeSize - *ptr1size;
		}

		m_fillcount += writeSize;
		m_writepos += writeSize;
		if (m_writepos>=m_maxsize)
			m_writepos -=m_maxsize;

		return writeSize;
	}

	int RingBuffer::skip(int size)
	{
		if (size>=m_fillcount)
		{
			int done = m_fillcount;
			clear();
			return done;
		}
		else if (size<=m_maxsize-m_readpos)
		{
			m_fillcount -= size;
			m_readpos += size;
			if (m_readpos>=m_maxsize)
				m_readpos = 0;
			return size;
		}
		else
		{
			m_fillcount -=size;
			m_readpos = size - (m_maxsize-m_readpos);
			return size;
		}

	}

	int RingBuffer::read(void *ptr, int size)
	{
		void* p1, *p2;
		int s1, s2;
		int done = read(size, &p1, &s1, &p2, &s2);

		if (s1)
		{
			memcpy(ptr, p1, s1);
			if (s2)
			{
				memcpy((char*)ptr + s1, p2, s2);
			}
		}

		return done;
	}

	int RingBuffer::getReadPointer(void **ptr)
	{
		int todo = std::min(m_fillcount, m_maxsize-m_readpos);
		*ptr =& m_pBuf[m_readpos];
		return todo;
	}

	int RingBuffer::getReadPointers(int readsize, void **ptr1, int *ptr1size, void **ptr2, int *ptr2size)
	{
		const int todo = std::min(m_fillcount, readsize);

		if (todo==0)
		{
			*ptr1 = 0;
			*ptr1size = 0;
			*ptr2 = 0;
			*ptr2size = 0;
			return 0;
		}

		if (m_readpos+todo<=m_maxsize)
		{
			*ptr1 = &m_pBuf[m_readpos];
			*ptr1size = todo;
			if (ptr2)
				*ptr2 = 0;
			if (ptr2size)
				*ptr2size = 0;
		}
		else
		{
			*ptr1 = &m_pBuf[m_readpos];
			*ptr1size = m_maxsize-m_readpos;
			if (ptr2)
				*ptr2 = &m_pBuf[0];
			if (ptr2size)
				*ptr2size = todo - *ptr1size;
		}

		return todo;
	}

	int RingBuffer::read(int readsize, void **ptr1, int *ptr1size, void **ptr2, int *ptr2size)
	{
		int done = getReadPointers(readsize, ptr1, ptr1size, ptr2, ptr2size);
		m_fillcount -=done;
		m_readpos += done;
		if (m_readpos>=m_maxsize)
			m_readpos -=m_maxsize;
		return done;
	}

	bool RingBuffer::peek(void* buf, int size)
	{
		void* ptr1;
		void* ptr2;
		int size1, size2;
		int done = getReadPointers(size, &ptr1, &size1, &ptr2, &size2);
		if (done!=size)
			return false;

		memcpy(buf, ptr1, size1);
		if (ptr2)
			memcpy(static_cast<char*>(buf)+size1, ptr2, size2);
		return true;
	}


} // namespace cz
