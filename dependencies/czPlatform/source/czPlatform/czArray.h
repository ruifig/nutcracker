/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com

	created:	2012/02/26
	created:	26:2:2012   1:40
	filename: 	czArray.h

	purpose:

*********************************************************************/
 /** @file */

#ifndef czArray_h__
#define czArray_h__

#include "czPlatformPrerequisites.h"

namespace cz
{

	/*!
		Creates/destroys array elements
	*/
	template<typename Type>
	struct TArrayElementCreation
	{
		static void constructCopy(void* at, const Type& val)
		{
			new(at, kOperatorPlacementNewDummy) Type(val);
		}

		static void constructCopy(void* at, const Type& val, int count)
		{
			while(count--)
			{
				new(at, kOperatorPlacementNewDummy) Type(val);
				at = reinterpret_cast<Type*>(at)+1;
			}
		}

		static void construct(void* at)
		{
			new(at, kOperatorPlacementNewDummy) Type();
		}

		template<typename A1>
		static void construct(void* at, A1 a1)
		{
			new(at, kOperatorPlacementNewDummy) Type(std::forward<A1>(a1));
		}

		template<typename A1, typename A2>
		static void construct(void* at, A1 a1, A2 a2)
		{
			new(at, kOperatorPlacementNewDummy) Type(std::forward<A1>(a1), std::forward<A2>(a2));
		}

		template<typename A1, typename A2, typename A3>
		static void construct(void* at, A1 a1, A2 a2, A3 a3)
		{
			new(at, kOperatorPlacementNewDummy) Type(std::forward<A1>(a1), std::forward<A2>(a2), std::forward<A3>(a3));
		}

		template<typename A1, typename A2, typename A3, typename A4>
		static void construct(void* at, A1 a1, A2 a2, A3 a3, A4 a4)
		{
			new(at, kOperatorPlacementNewDummy) Type(std::forward<A1>(a1), std::forward<A2>(a2), std::forward<A3>(a3), std::forward<A4>(a4));
		}

		template<typename A1, typename A2, typename A3, typename A4, typename A5>
		static void construct(void* at, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5)
		{
			new(at, kOperatorPlacementNewDummy) Type(std::forward<A1>(a1), std::forward<A2>(a2), std::forward<A3>(a3), std::forward<A4>(a4), std::forward<A5>(a5));
		}

		static void construct_multiple(void* at, int count)
		{
			while(count--)
			{
				new(at, kOperatorPlacementNewDummy) Type();
				at = reinterpret_cast<Type*>(at)+1;
			}
		}

		static void destroy(void* at)
		{
			(reinterpret_cast<Type*>(at))->~Type();
#if CZ_DEBUG
			memset(at, 0xDD, sizeof(Type));
#endif
		}

		// Destruction is done backwards. This matches the way C++ native arrays work.
		static void destroy(void* at, int count)
		{
			Type *p = reinterpret_cast<Type*>(at)+(count-1);
			int todo = count;
			while(todo--)
			{
				p->~Type();
				p--;
			}

#if CZ_DEBUG
			memset(at, 0xDD, sizeof(Type)*count);
#endif
		}

	};


	//
	// Base template class for arrays.
	// Only contains static methods.
	template <typename T>
	class TBaseArray
	{
	public:
		typedef T Type;

	protected:

		/*! Finds an element inside the specified range
		 * \param start Where to start looking
		 * \param end Where to stop looking (exclusive). This should point to the element AFTER the last one you want to check
		 * \param val Value to search for
		 * \param index index of the element, if found
		 * \return true if found, false otherwise
		 */
		static const bool _find(const Type* start, const Type* end, const Type &val, int &index)
		{
			CZ_ASSERT(start<=end);

			const Type *p = start;
			while(p<end)
			{
				if (*p==val)
				{
					index = static_cast<int>(p - start);
					return true;
				}
				p++;
			}

			return false;
		}

		/*! Finds an element inside the specified range
		*/
		static const bool _find(const Type* start, const Type* end, const Type &val)
		{
			CZ_ASSERT(start<=end);

			const Type *p = start;
			while(p<end)
			{
				if (*p==val)
				{
					return true;
				}
				p++;
			}

			return false;
		}


		/*!
		 * \param at Where to insert the the new value
		 * \param end Where the array ends (exclusive). This should point to the element AFTER the last one
		 * \param val Value to insert
		 */
		static void _insertAt(Type *at, Type* end, const Type &val)
		{
			CZ_ASSERT(at<=end);

			// Move elements forward to make space for the new one
			size_t count = end-at;
			if (count)
			{
				Type *p=end;
				while(count--)
				{
					TArrayElementCreation<Type>::constructCopy(p,*(p-1));
					p--;
					TArrayElementCreation<Type>::destroy(p);
				}
			}

			TArrayElementCreation<Type>::constructCopy(at, val);
		}

		/*!
		* \param at Where to insert the the new value
		* \param end Where the array ends (exclusive). This should point to the element AFTER the last one
		*/
		static void _removeAt(Type *at, Type *end)
		{
			CZ_ASSERT(at<end);

			TArrayElementCreation<Type>::destroy(at);

			// Move the other items, compacting the array
			int count = static_cast<int>(end-at-1);
			Type *p = at;
			while(count--)
			{
				TArrayElementCreation<Type>::constructCopy(p, *(p+1));
				p++;
				TArrayElementCreation<Type>::destroy(p);
			}

		}

		/*!
		* Moves count objects from pSrc to pDst.
		* The source objects will be destroyed
		*
		* \param dst Uninitialized memory, where the new objects will be construct
		* \param src pointer to the first object to copy.
		* \param count how many elements to move
		*
		*/
		static void _moveToNewMemory(void *dst, Type *src, int count)
		{
			while(count--)
			{
				TArrayElementCreation<Type>::constructCopy(dst, *src);
				TArrayElementCreation<Type>::destroy(src);
				dst = reinterpret_cast<Type*>(dst)+1;
				src++;
			}
		}

		/*!
		* Create "count" copies objects from pSrc in pDst.
		*
		* \param dst Uninitialized memory, where the new objects will be construct
		* \param src pointer to the first object to copy.
		* \param count how many elements to copy
		*
		*/
		static void _copyToNewMemory(void *dst, const Type *src, int count)
		{
			while(count--)
			{
				TArrayElementCreation<Type>::constructCopy(dst, *src);
				dst = reinterpret_cast<Type*>(dst)+1;
				src++;
			}
		}

		/*! Sets a range of elements to the specified value
		*/
		static void _setTo(Type* start, const Type &val, int count)
		{
			while(count--)
			{
				*start = val;
				start++;
			}
		}

	};



#ifdef __clcpp_parse__

	template <typename Type > class TStaticArray : TBaseArray<Type>
	{
	};

	#define TSTATICARRAY(Type, size, tracksize) cz::TStaticArray<Type>

#else
	/*! Thin wrapper around a simple static array, to make it easier to manipulate and perform index checks
	 \tparam Type Element type
	 \tparam Size Size of the array
	 \tparam TrackUsedSize
		If this is true, then it will simulate a dynamic array, by adding an extra member variable to keep track of how many elements the array has.
		If false, then it behaves exactly as a C array, so no overhead.
	*/
	//!
	// Thin wrapper around a simple array, to make it easier to manipulate, and keep the array
	// size along with it.
	//
	template
		<
		typename Type,
		int Size,
		bool TrackUsedSize=false
		> class TStaticArray
	{
	};

	/*! Specialization of TStaticArray for TrackUsedSize=false, so it behaves as a C style array
	*/
	template<typename Type, int Size>
	class TStaticArray<Type, Size, false> : public TBaseArray<Type>
	{
	public:

		enum
		{
			SIZE = Size
		};

		/*! */
		TStaticArray()
		{
			// Compile time assert to make sure the size of this object is the same size as the equivalent C style array
			CZ_COMPILETIME_ASSERT(sizeof(TStaticArray)==sizeof(Type)*SIZE);
			TArrayElementCreation<Type>::construct_multiple(&eleAt(0), SIZE);
		}

		/*! */
		~TStaticArray()
		{
			TArrayElementCreation<Type>::destroy(&eleAt(0), SIZE);
		}

		/*! */
		const Type& operator[](int index) const
		{
			CZ_ASSERT(index<SIZE);
			return eleAt(index);
		}

		/*! */
		Type& operator[](int index)
		{
			CZ_ASSERT(index<SIZE);
			return eleAt(index);
		}

		/*! Sets all elements of the array to the specified value
		*/
		void setAll(const Type &val)
		{
			_setTo(&eleAt(0), val, SIZE);
		}

		//! Finds the specified value, and lets you know at what index
		// \param val Value to search for
		// \param destIndex Where you'll get the index at which the value was found
		// \return true if found (destIndex will contain the index), false if not found
		// \note Complexity is O(n). It just transverses the array to find the value
		bool find(const Type &val, int &destIndex) const
		{
			return _find(&eleAt(0), &eleAt(SIZE), val, destIndex);
		}

		/*! */
		bool find(const Type &val) const
		{
			return _find(&eleAt(0), &eleAt(SIZE), val);
		}

		Type* begin()
		{
			return &eleAt(0);
		}

		Type* end()
		{
			return &eleAt(SIZE);
		}

		/*! Returns the array size */
		int size() const
		{
			return SIZE;
		}

		int capacity() const
		{
			return SIZE;
		}

	protected:
		const Type& eleAt(int index) const
		{
			return *(reinterpret_cast<const Type*>(&mItems[sizeof(Type)*index]));
		}

		Type& eleAt(int index)
		{
			return *(reinterpret_cast<Type*>(&mItems[sizeof(Type)*index]));
		}

		// Using an array of raw memory, because object creation/destruction is done manually
		char mItems[sizeof(Type)*SIZE];
	private:

		// Private to avoid use
		TStaticArray(const TStaticArray& other);
		TStaticArray& operator=(const TStaticArray& other);
	};


	/*! Specialization of TStaticArray for TrackUsedSize=true, so it simulates a dynamic array as close as possible
	*/
	template<typename Type, int Size>
	class TStaticArray<Type, Size, true> : public TBaseArray<Type>
	{
	public:

		TStaticArray() : mUsedSize(0)
		{
#if CZ_DEBUG
			memset(mItems, 0xCD, sizeof(Type)*SIZE);
#endif
		}

		~TStaticArray()
		{
			clear();
		}

		/*! Removes all elements from the array
		 */
		void clear()
		{
			TArrayElementCreation<Type>::destroy(&eleAt(0), mUsedSize);
			mUsedSize = 0;
		}

		/*! Puts a new element into the end of the array
		\return true if succeeded, false otherwise
		*/
		bool push(const Type &val)
		{
			if (mUsedSize==SIZE)
			{
				return false;
			}
			else
			{
				TArrayElementCreation<Type>::constructCopy(&eleAt(mUsedSize), val);
				mUsedSize++;
				return true;
			}
		}

		/*! Same as \link push \endlink , so it has an interface similar to std::vector*/
		bool push_back(const Type &val)
		{
			return push(val);
		}

		/*! Removes the last element
		\return true if succeeded, false if the array was empty
		*/
		bool pop()
		{
			if (mUsedSize==0)
			{
				return false;
			}
			else
			{
				mUsedSize--;
				TArrayElementCreation<Type>::destroy(&eleAt(mUsedSize));
				return true;
			}
		}

		/*! Removes the last element
		\param dest Where you'll get the popped element
		\return true if succeeded, false if the array was empty
		*/
		bool pop(Type &dest)
		{
			if (mUsedSize==0)
			{
				return false;
			}
			else
			{
				dest = eleAt(mUsedSize-1);
				return pop();
			}
		}

		/*! Same as \link pop \endlink, so it has an interface similar to std::vector
		*/
		bool pop_back()
		{
			return pop();
		}

		/*! */
		const Type& operator[](int nIndex) const
		{
			CZ_ASSERT(nIndex<mUsedSize);
			return eleAt(nIndex);
		}

		/*! */
		Type& operator[](int nIndex)
		{
			CZ_ASSERT(nIndex<mUsedSize);
			return eleAt(nIndex);
		}

		/*! returns a reference to the last element */
		const Type& last() const
		{
			CZ_ASSERT(mUsedSize>0);
			return eleAt(mUsedSize-1);
		}

		/*! returns a reference to the last element */
		Type& last()
		{
			CZ_ASSERT(mUsedSize>0);
			return eleAt(mUsedSize-1);
		}

		Type* begin()
		{
			return &eleAt(0);
		}

		Type* end()
		{
			return &eleAt(mUsedSize);
		}

		/*! Returns the number of indexes used in the array */
		int size() const
		{
			return mUsedSize;
		}

		int capacity() const
		{
			return SIZE;
		}

		/*! Tells if the array is empty or not */
		bool empty() const
		{
			return (mUsedSize==0) ? true : false;
		}

		/*! Finds an element
		\param val Element to find
		\param destIndex index of the element
		\return true if found, false otherwise
		*/
		bool find(const Type &val, int &destIndex) const
		{
			return _find(&eleAt(0), &eleAt(mUsedSize), val, destIndex);
		}

		/*! Finds an element
		\param val Element to find
		\return true if found, false otherwise
		*/
		bool find(const Type &val) const
		{
			return _find(&eleAt(0), &eleAt(mUsedSize), val);
		}

		/*! Inserts the specified value at the specified index, moving forward all the elements, starting at index "index"
		\param index Index at which to insert
		\param val Value to insert
		\return true if succeeded, false if index is invalid, or array is full
		\warning Just like the removeAtIndex methods, this method moves all the elements after the specified index, so should be used with care and with small arrays.
		*/
		bool insertAtIndex(int index, const Type &val)
		{
			if (mUsedSize==SIZE || index>mUsedSize)
			{
				return false;
			}

			_insertAt(&eleAt(index), &eleAt(mUsedSize), val);
			mUsedSize++;
			return true;
		}


		/*! Removes the element at the specified index, and compacts the array.
		\param index index to remove
		\return true if succeed, false if the index is invalid
		\warning This function moves all elements after the specified index, it can be slow if used with big arrays.
		*/
		bool removeAtIndex(int index)
		{
			if (index<0 || index>=mUsedSize)
			{
				return false;
			}

			_removeAt(&eleAt(index), &eleAt(mUsedSize));
			mUsedSize--;
			return true;
		}

		/*! Removes the element at the specified index, and compacts the array.
		\param index index to remove
		\param dest Where you'll get the removed value
		\return true if succeed, false if the index is invalid
		\warning This function moves all elements after the specified index, it can be slow if used with big arrays.
		*/
		bool removeAtIndex(int index, Type &dest)
		{
			if (index<0 || index>=mUsedSize)
			{
				return false;
			}

			dest = eleAt(index);
			return removeAtIndex(index);
		}

		/*! Removes any copies of the specified element, if they exist
		\param val Value to remove
		\return number of elements removed
		*/
		int removeIfExists(const Type &val)
		{
			int count=0;
			for (int i=size(); i>0; i--)
			{
				if (eleAt(i-1)==val)
				{
					count++;
					removeAtIndex(i-1);
				}
			}
			return count;
		}

	protected:
		enum
		{
			SIZE = Size
		};

		const Type& eleAt(int index) const
		{
			return *(reinterpret_cast<const Type*>(&mItems[sizeof(Type)*index]));
		}

		Type& eleAt(int index)
		{
			return *(reinterpret_cast<Type*>(&mItems[sizeof(Type)*index]));
		}

		// Using raw memory, because object creation/destruction is controlled manually
		char mItems[sizeof(Type)*SIZE];
		int mUsedSize;

	private:
		// Private to avoid use
		TStaticArray(const TStaticArray& other);
		TStaticArray& operator=(const TStaticArray& other);
	};

	#define TSTATICARRAY(Type, size, tracksize) cz::TStaticArray<Type, size, tracksize>

#endif


	/*! Dynamic array
	*/
	template<typename Type>
	class TArray : public TBaseArray<Type>
	{
	public:

		TArray()
		{
			mData = NULL;
			mCapacity = 0;
			mSize = 0;
		}

		/*! Constructor
		\param initialCapacity Initial array capacity
		*/
		explicit TArray(long initialCapacity)
		{
			// Don't allow capacity 0, otherwise we would have a NULL pointer, and iterators wouldn't work
			if (initialCapacity>0)
			{
				mData = CZ_ALLOC(initialCapacity*sizeof(Type), MemCat_General);
				mCapacity = initialCapacity;
				mSize = 0;
				#if CZ_DEBUG
				memset(mData, 0xCD, sizeof(Type)*mCapacity);
				#endif
			}
			else
			{
				mData = NULL;
				mCapacity = 0;
				mSize = 0;
			}

		}

		/*! Construct the array, copying from another array*/
		TArray(const TArray& other)
		{
			mData = NULL;
			mCapacity = 0;
			mSize = 0;
			setTo(other);
		}

		~TArray()
		{
			clear();
			if (mData)
			{
				CZ_FREE(mData, MemCat_General);
			}
		}


		/*! \name STL compatible methods
			@{
		*/

		/*! Returns a pointer to the first element
		This can be used with std algorithms.
		\note in order to work with stl algorithms, if the array is empty, it will return NULL (which means it's equal to end())
		*/
		Type* begin()
		{
			return (mData) ? &eleAt(0) : NULL;
		}

		const Type* begin() const
		{
			return (mData) ? &eleAt(0) : NULL;
		}
	
		/*! Returns a pointer addressing the position one past the final element
		This can be used with std algorithms.
		\note In order to work with stl algorithms, if the array is empty, it will return NULL (which will be equal to begin())
		*/
		Type* end()
		{
			return (mData) ? &eleAt(mSize) : NULL;
		}

		const Type* end() const
		{
			return (mData) ? &eleAt(mSize) : NULL;
		}

		/*! Grows the array if necessary, to have enough capacity for the specified number of elements */
		void reserve(int newcapacity)
		{
			if (newcapacity>mCapacity)
				grow(newcapacity);
		}

		/*! Resizes the container to contain count elements.
		If the current size is less than count, additional elements are appended and initialized with value.
		If the current size is greater than count, the container is reduced to its first count elements.
		*/
		void resize(int count)
		{
			if (size()<count)
			{
				while(size()!=count)
					emplace_back();
			}
			else
			{
				while(size()>count)
					pop_back();
			}
		}

		/*! Reduces the capacity to "size"
		This can be used to release unused memory
		*/
		void shrink_to_fit()
		{
			setCapacity(mSize);
		}

		/*! Returns how many elements the array can contain without allocating more memory */
		int capacity() const
		{
			return mCapacity;
		}

		/*! Same as push
		This is to improve std::vector compatibility
		*/
		void push_back(const Type& val)
		{
			push(val);
		}

		/*! Remove the last element, if any */
		void pop_back()
		{
			pop();
		}


		/* Appends a new element at the end of the array, constructing it in-place*/
		void emplace_back()
		{
			if (mSize==mCapacity)
				grow(mCapacity*2);

			TArrayElementCreation<Type>::construct(&eleAt(mSize));
			mSize++;
		}

		template<typename A1>
		void emplace_back(A1 a1)
		{
			if (mSize==mCapacity)
				grow(mCapacity*2);

			TArrayElementCreation<Type>::construct(&eleAt(mSize), std::forward<A1>(a1));
			mSize++;
		}


		template<typename A1, typename A2>
		void emplace_back(A1 a1, A2 a2)
		{
			if (mSize==mCapacity)
				grow(mCapacity*2);

			TArrayElementCreation<Type>::construct(&eleAt(mSize), std::forward<A1>(a1), std::forward<A2>(a2));
			mSize++;
		}

		template<typename A1, typename A2, typename A3>
		void emplace_back(A1 a1, A2 a2, A3 a3)
		{
			if (mSize==mCapacity)
				grow(mCapacity*2);

			TArrayElementCreation<Type>::construct(&eleAt(mSize), std::forward<A1>(a1), std::forward<A2>(a2), std::forward<A3>(a3));
			mSize++;
		}

		template<typename A1, typename A2, typename A3, typename A4>
		void emplace_back(A1 a1, A2 a2, A3 a3, A4 a4)
		{
			if (mSize==mCapacity)
				grow(mCapacity*2);

			TArrayElementCreation<Type>::construct(&eleAt(mSize), std::forward<A1>(a1), std::forward<A2>(a2), std::forward<A3>(a3), std::forward<A4>(a4));
			mSize++;
		}

		template<typename A1, typename A2, typename A3, typename A4, typename A5>
		void emplace_back(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5)
		{
			if (mSize==mCapacity)
				grow(mCapacity*2);

			TArrayElementCreation<Type>::construct(&eleAt(mSize), std::forward<A1>(a1), std::forward<A2>(a2), std::forward<A3>(a3), std::forward<A4>(a4), std::forward<A5>(a5));
			mSize++;
		}

		/*! Returns how many elements there are in the array */
		int size() const
		{
			return mSize;
		}

		/*! Removes all elements from the array */
		void clear()
		{
			if (mSize>0)
			{
				TArrayElementCreation<Type>::destroy(&eleAt(0), mSize);
				mSize = 0;
			}
		}


		const Type& front() const { return operator[](0); }
		Type& front() { return operator[](0); }
		const Type& back() const { return last(); }
		Type& back() { return last(); }

		/*!
			@}
		*/


		/*! \name Operators
			@{
		*/

		/*! */
		TArray& operator=(const TArray& other)
		{
			if (this!=&other)
				setTo(other);
			return *this;
		}

		/*! */
		const Type& operator[](int index) const
		{
			CZ_ASSERT(index<mSize);
			return eleAt(index);
		}

		/*! */
		Type& operator[](int index)
		{
			CZ_ASSERT(index<mSize);
			return eleAt(index);
		}

		bool operator==(const TArray& other) const
		{
			if (this==&other)
				return true;
			if (this->size() != other.size())
				return false;

			for(int i=0; i<size(); i++)
			{
				if (!(eleAt(i)==other.eleAt(i)))
					return false;
			}
			return true;
		}

		bool operator<(const TArray& other) const
		{
			// Not really the same behaviour as in std::vector
			return size() < other.size();
		}
		/*!
			@}
		*/

		/*! Adds a new element to the end of the array
		\param val Element to add
		\return true if successful, false otherwise (e.g: Out of memory)*/
		bool push(const Type& val)
		{
			if (mSize==mCapacity)
			{
				if (!grow(mCapacity*2))
					return false;
			}

			TArrayElementCreation<Type>::constructCopy(&eleAt(mSize), val);
			mSize++;
			return true;
		}

		/*! Adds copies an an element to the end of the array
		\param val Element to add
		\param count how many copies to add
		\return true if successful, false otherwise (e.g: Out of memory)*/
		bool push(const Type& val, int count)
		{
			while (count--)
			{
				if (!push(val))
					return false;
			}
			return true;
		}


		/*! Removes the last element from the array
		\param dest Where you'll get the element
		\return true on success, false if the array was empty
		*/
		bool pop(Type& dest)
		{
			if (mSize==0)
				return false;
			mSize--;
			dest = eleAt(mSize);
			TArrayElementCreation<Type>::destroy(&eleAt(mSize));
			return true;
		}

		/*! Removes the last element from the array
		\return true on success, false if the array was empty
		*/
		bool pop()
		{
			if (mSize==0)
				return false;
			mSize--;
			TArrayElementCreation<Type>::destroy(&eleAt(mSize));
			return true;
		}

		/*! Returns a reference to the last element
		*/
		const Type& last() const
		{
			CZ_ASSERT(mSize>0);
			return eleAt(mSize-1);
		}

		/*! Returns a reference to the last element
		*/
		Type& last()
		{
			CZ_ASSERT(mSize>0);
			return eleAt(mSize-1);
		}

		/*! Finds an element
		\param val Value to search for
		\param destIndex Where you'll get the index at which the value was found
		\return true if found (destIndex will contain the index), false if not found
		\note Complexity is O(n). It just transverses the array to find the value
		*/
		bool find(const Type &val, int &destIndex) const
		{
			return _find(&eleAt(0), &eleAt(mSize), val, destIndex);
		}

		/*! Finds an element*/
		bool find(const Type &val) const
		{
			return _find(&eleAt(0), &eleAt(mSize), val);
		}


		/*! Inserts a new element at the specified position
		*/
		bool insertAtIndex(int index, const Type &val)
		{
			if (index>mSize)
			{
				return false;
			}

			if (mSize==mCapacity)
			{
				if (!grow(mCapacity*2))
					return false;
			}

			_insertAt(&eleAt(index), &eleAt(mSize), val);
			mSize++;
			return true;
		}


		/*! Removes the element at the specified index, and compacts the array.
		\param index index to remove
		\return true if succeed, false if the index is invalid
		\warning This method moves all the elements after the specified index, so it can be slow with big arrays.
		*/
		bool removeAtIndex(int index)
		{
			if (index<0 || index>=mSize)
			{
				return false;
			}

			_removeAt(&eleAt(index), &eleAt(mSize));
			//The last element is now invalid, so decrease the size by 1
			mSize--;
			return true;
		}

		/*! Does the same thing as RemoveAtIndex, but instead of moving all the elements back, it simply copies the last one to that position.
			Its a lot faster, but you should only use this function if the element order is not important.
		*/
		bool removeAtIndexAndFillWithLast(int index)
		{
			if (index<0 || index>=mSize)
			{
				return false;
			}

			Type* pAt = &eleAt(index);
			TArrayElementCreation<Type>::destroy(pAt);
			// Copy the last element to the position we want to delete
			if (index<mSize-1)
			{
				Type* pLastEle = &eleAt(mSize-1);
				TArrayElementCreation<Type>::constructCopy(pAt, *pLastEle);
				TArrayElementCreation<Type>::destroy(pLastEle);
			}

			mSize--;

			return true;

		}

		/*! Removes any copies of the specified value
		\param val Value to remove
		\return number of elements removed
		*/
		int removeIfExists(const Type &val)
		{
			int count=0;
			for (int i=size(); i>0; i--)
			{
				if (eleAt(i-1)==val)
				{
					count++;
					removeAtIndex(i-1);
				}
			}
			return count;
		}


		/*! Appends data from another array
		*/
		void append(const TArray& other)
		{
			// This actually supports appending an array to itself
			append(other.ptrToEleAt(0), other.size());
		}

		void append(const Type* data, int count)
		{
			reserve(size()+count);
			_copyToNewMemory(ptrToEleAt(size()), data, count);
			mSize += count;
		}

	private:
		const Type& eleAt(int index) const
		{
			return *(reinterpret_cast<const Type*>(&( ((char*)mData)[sizeof(Type)*index] )));
		}

		Type& eleAt(int index)
		{
			return *(reinterpret_cast<Type*>(&( ((char*)mData)[sizeof(Type)*index] )));
		}

		const Type* ptrToEleAt(int index) const
		{
			return reinterpret_cast<const Type*>(&( ((char*)mData)[sizeof(Type)*index] ));
		}

		Type* ptrToEleAt(int index)
		{
			return reinterpret_cast<Type*>(&( ((char*)mData)[sizeof(Type)*index] ));
		}

		void setTo(const TArray& other)
		{
			clear();
			if (other.mSize)
			{
				reserve(other.mCapacity);
				for (int i=0; i<other.size(); i++)
					push(other[i]);
			}
		}

		// Can be used to grow or shrink the allocated memory
		void setCapacity(int newCapacity)
		{
			CZ_ASSERT(newCapacity>=mSize);

			void* pNew = NULL;

			if (newCapacity)
			{
				pNew = CZ_ALLOC(newCapacity*sizeof(Type), MemCat_General);
#if CZ_DEBUG
				memset(pNew, 0xCD, sizeof(Type)*newCapacity);
#endif
				_moveToNewMemory(pNew, &eleAt(0), mSize);
			}

			if (mData)
			{
				CZ_FREE(mData, MemCat_General);
			}

			mData = pNew;
			mCapacity = newCapacity;
		}

		bool grow(int newCapacity)
		{
			// This is needed because because if we construct the array with initial capacity 0,
			// then we end up calling this function with nNewCapaticy 0
			if (newCapacity==0)
				newCapacity = 16;

			CZ_ASSERT(newCapacity>mCapacity);

			setCapacity(newCapacity);
			return true;
		}

		void* mData;
		int mCapacity;
		int mSize;
	};

} // namespace cz

#endif // czArray_h__
