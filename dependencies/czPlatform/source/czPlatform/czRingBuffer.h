/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com
	
	purpose:
	
*********************************************************************/

#ifndef _czRingBuffer_h_
#define _czRingBuffer_h_

#include "czPlatformPrerequisites.h"

namespace cz
{


	class RingBuffer
	{
	public:
		RingBuffer();
		~RingBuffer();

		//! Manually grows to the specified max size, if the current max size is smaller
		void reserve(int size);
		int getUsedSize() const
		{
			return m_fillcount;
		}
		int getMaxSize() const
		{
			return m_maxsize;
		}
		int getFreeSize() const
		{
			return m_maxsize - m_fillcount;
		}


		//! Empties the buffer
		/*
		 * \param releaseMemory
		 *    If true, it will free any allocated resources, which means it will need to allocate
		 *    memory again when witting data to the buffer.
 		 */
		void clear(bool releaseMemory=false);

		//! Writes data to the buffer
		/*!
		 * \param ptr Data to write
		 * \param size how many bytes to write
		 * \return number of bytes written
		 * \note If necessary, the buffer will expand to accept the new data
		 */
		int write(const void *ptr, int size);

		//! Simulates a write, and give you the pointers you can use to write the data yourself
		/*!
		 */
		int write(int writeSize, void** ptr1, int* ptr1size, void **ptr2, int* ptr2size);

		//! Reads data from the buffer
		/*
		 *
		 * \param ptr where you get the data
		 * \param size how many bytes to read
		 * \return the number of bytes actually read
		 */
		int read(void *ptr, int size);

		//! \brief Returns the internal buffer for read pointer
		/*!
		 * You can use this function to get a pointer to the internal buffer, without worrying about
		 * the wrap around, as the buffer returned is what you can use without wrapping around.
		 * \param ptr Where you get the pointer
		 * \return The number of bytes you can read from the pointer, or 0 if nothing to read.
		 */
		int getReadPointer(void **ptr);

		//!
		/*!
		 * \brief Returns the internal buffers for the specified read operation, without removing data from the buffer
		 *
		 * \param readsize How many bytes you want to read
		 * \param ptr1 Where you'll get the pointer to the first part
		 * \param ptr1size Here you'll get how many bytes you can read from ptr1
		 * \param ptr2 Where you'll get the pointer to the first part, or NULL if everything is available with ptr1
		 * \param ptr2size Here you'll get how many bytes you can read from ptr2
		 * \param size size you want to read
		 * \return
		 *   Number of bytes you can actually read from the returned pointers. If it's smaller than the readsize,
		 *   it means there wasn't enough bytes available in the buffer. 
		 *
		 * \note
		 *  This returns pointers to the internal memory, for read only, which is handy in some cases to avoid some unnecessary memory copying
		 *  Therefore you shouldn't keep those pointers. Those pointers should be considered valid only until another method is called on the buffer
		 */
		int getReadPointers(int readsize, void **ptr1, int *ptr1size, void **ptr2, int *ptr2size);

		//!
		/*!
		 * \brief Returns the internal buffers pointers for the specified read operation
		 *
		 * This function is useful if you want to avoid some memory copying when reading from the buffer.
		 *
		 * \param readsize How many bytes you want to read
		 * \param ptr1 Where you'll get the pointer to the first part
		 * \param ptr1size Here you'll get how many bytes you can read from ptr1
		 * \param ptr2 Where you'll get the pointer to the first part, or NULL if everything is available with ptr1
		 * \param ptr2size Here you'll get how many bytes you can read from ptr2
		 * \param size size you want to read
		 * \return
		 *   Number of bytes you can actually read from the returned pointers. If it's smaller than the readsize,
		 *   it means there wasn't enough bytes available in the buffer. 
		 *
		 * \note
		 *  This returns pointers to the internal memory, for read only, which is handy in some cases to avoid some unnecessary memory copying
		 *  Therefore you shouldn't keep those pointers. Those pointers should be considered valid only until another method is called on the buffer
		 */
		int read(int readsize, void **ptr1, int *ptr1size, void **ptr2, int *ptr2size);

		//! Skips the specified amount of bytes
		/*!
		 * \param size How many bytes to dump
		 * \return how many bytes were actually dumped
		 */
		int skip(int size);


		/*
		Templated write function, to make it easier to write primitive types
		*/
		template<typename T>
		int write(T v)
		{
			return write(&v, sizeof(v));
		}

		/*
		Templated read function, to make it easier to read primitive types
		*/
		template<typename T>
		int read(T* v)
		{
			return read(v, sizeof(*v));
		}


		/*
		Peek at data, without removing it from the buffer.
		Returns true if successful, false if there is enough data in the buffer
		*/
		template<typename T>
		bool peek(T* v)
		{
			return peek(v, sizeof(*v));
		}

	private:

		// Private to avoid use
		RingBuffer(const RingBuffer& other);
		void operator=(const RingBuffer& other);

		bool peek(void* buf, int size);

		char *m_pBuf;
		int m_maxsize;
		int m_fillcount;
		int m_readpos;
		int m_writepos;
	};

} // namespace cz

#endif // _czRingBuffer_h_
