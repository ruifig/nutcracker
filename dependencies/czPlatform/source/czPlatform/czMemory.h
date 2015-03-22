/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com

	created:	2012/02/10
	created:	10:2:2012   0:51
	filename: 	czMemory.h

	purpose:
	Memory allocation related code and macros
*********************************************************************/

/** @file */


#ifndef czMemory_h__
#define czMemory_h__


#include "czPlatformPrerequisites.h"

#include "czMemoryCategories.h"

namespace cz
{

/** \addtogroup Memory Memory
	Most of the memory management/tracking was inspired by Ogre ( http://www.ogre3d.org/ )
	@{
*/

	/*! Interface to allocate memory at the lowest level.
	An object of this type can be passed to \link PlatformRoot \endlink to specify your own memory
	allocation.
	*/
	class ApplicationMemoryAllocator
	{
	public:
        virtual ~ApplicationMemoryAllocator() {};

		/*! Allocates a block of raw memory.
		The extra debug details (file, line, func) are passed if available. If not available,
		they'll be NULL/0.
		*/
		virtual void* allocate(size_t size, const char* file, int line, const char* func) = 0;
		/*! Free a block of memory */
		virtual void deallocate(void* ptr) = 0;
	};

	// Just to move the allocation to the CPP file, to avoid adding dependencies to
	// PlatformRoot, MemoryTracker, etc
	class CategoryAllocatorBase
	{
	protected:
		static void* _allocateBytes(size_t size, int category, const char* file=NULL, int line=0, const char* func=NULL);
		static void _deallocateBytes(void* ptr);
		static size_t _getMaxAllocationSize();
	};


#ifdef __clcpp_parse__

	class CategoryAllocator
	{
	public:
		static void* allocateBytes(size_t size, const char* file=NULL, int line=0, const char* func=NULL)
		{
			return NULL;
		}
		static void deallocateBytes(void* ptr)
		{			
		}

		static size_t getMaxAllocationSize()
		{
			return 0;
		}
	};

	typedef CategoryAllocator GeneralCategoryAllocator;
	typedef CategoryAllocator StringCategoryAllocator;

#else
	/** Default allocator for all categories
	You can specialize this template for each category
	*/
	template<MemoryCategory memcat>
	class CategoryAllocator : CategoryAllocatorBase
	{
	public:
		static void* allocateBytes(size_t size, const char* file=NULL, int line=0, const char* func=NULL)
		{
			return _allocateBytes(size, memcat, file, line, func);
		}
		static void deallocateBytes(void* ptr)
		{
			_deallocateBytes(ptr);
		}

		static size_t getMaxAllocationSize()
		{
			return _getMaxAllocationSize();
		}
	};

	typedef CategoryAllocator<MemCat_General> GeneralCategoryAllocator;
	typedef CategoryAllocator<MemCat_String> StringCategoryAllocator;
#endif

	/** Base class for memory tracked objects.
	*/
	template<class Allocator >
	class TAllocatedObject
	{
	public:
		explicit TAllocatedObject()
		{
		}
		~TAllocatedObject()
		{
		}

		void* operator new(size_t size, const char* file, int line, const char* func)
		{
			return Allocator::allocateBytes(size, file, line, func);
		}

		void* operator new(size_t size)
		{
			return Allocator::allocateBytes(size, NULL, 0, NULL);
		}

		// placement operator new
		void* operator new(size_t, void* ptr)
		{
			return ptr;
		}

		/// array operator new
		void* operator new[] ( size_t size, const char* file, int line, const char* func )
		{
			return Allocator::allocateBytes(size, file, line, func);
		}

		void* operator new[] ( size_t size )
		{
			return Allocator::allocateBytes(size, NULL, 0, NULL);
		}

		void operator delete( void* ptr )
		{
			Allocator::deallocateBytes(ptr);
		}

		// Corresponding operator for placement delete (second param same as the first)
		void operator delete( void* ptr, void* )
		{
		}
		void operator delete( void* ptr, void*, unsigned long) // match new(size_t, void* ptr, unsigned long)
		{
		}

		// this is needed in case there is an exception throw from inside new.
		void operator delete( void* ptr, const char* /*file*/ , int /*line*/ , const char* /*func*/ )
		{
			Allocator::deallocateBytes(ptr);
		}

		void operator delete[] ( void* ptr )
		{
			Allocator::deallocateBytes(ptr);
		}

		void operator delete[] ( void* ptr, const char* /*file*/ , int /*line*/, const char* /*func*/)
		{
			Allocator::deallocateBytes(ptr);
		}
	};


	template <typename T>
	struct STLAllocatorBase
	{
		typedef T value_type;
	};

	template <typename T>
	struct STLAllocatorBase<const T>
	{
		typedef T value_type;
	};


	/*! Allocator for STL containers
	*/
	template <typename T, typename CatAlloc>
	class STLAllocator : public STLAllocatorBase<T>
	{
	public:
		typedef STLAllocatorBase<T>			Base;
		typedef typename Base::value_type	value_type;
		typedef value_type*					pointer;
		typedef const value_type*			const_pointer;
		typedef value_type&					reference;
		typedef const value_type&			const_reference;
		typedef size_t						size_type;
		typedef std::ptrdiff_t				difference_type;


		/// the standard rebind mechanism
		template<typename U>
		struct rebind
		{
			typedef STLAllocator<U, CatAlloc> other;
		};

		/// ctor
		inline explicit STLAllocator()
		{ }

		/// dtor
		virtual ~STLAllocator()
		{ }

		/// copy ctor - done component wise
		inline STLAllocator( STLAllocator const& )
		{ }

		/// cast
		template <typename U>
		inline STLAllocator( STLAllocator<U, CatAlloc> const& )
		{ }

		/// cast
		template <typename U, typename P>
		inline STLAllocator( STLAllocator<U, P> const& )
		{ }

		/// memory allocation (elements, used by STL)
		inline pointer allocate( size_type count)
		{
			// convert request to bytes
			size_type sz = count*sizeof( T );
			pointer p  = static_cast<pointer>(CatAlloc::allocateBytes(sz));
			return p;
		}

		/// memory deallocation (elements, used by STL)
		inline void deallocate( pointer ptr, size_type )
		{
			CatAlloc::deallocateBytes(ptr);
		}

		pointer address(reference x) const
		{
			return &x;
		}

		const_pointer address(const_reference x) const
		{
			return &x;
		}

		size_type max_size() const throw()
		{
			return CatAlloc::getMaxAllocationSize();
		}

		void construct(pointer p, const T& val)
		{
			// call placement new
			new(static_cast<void*>(p)) T(val);
		}

#pragma warning(push, 0) // disable all warnings
		void destroy(pointer p)
		{
			p->~T();
		}
#pragma warning(pop)
	};

	/// determine equality, can memory from another allocator
	/// be released by this allocator, (ISO C++)
	template<typename T, typename T2, typename P>
	inline bool operator==(STLAllocator<T,P> const&,
		STLAllocator<T2,P> const&)
	{
		// same alloc policy (P), memory can be freed
		return true;
	}

	/// determine equality, can memory from another allocator
	/// be released by this allocator, (ISO C++)
	template<typename T, typename P, typename OtherAllocator>
	inline bool operator==(STLAllocator<T,P> const&,
		OtherAllocator const&)
	{
		return false;
	}
	/// determine equality, can memory from another allocator
	/// be released by this allocator, (ISO C++)
	template<typename T, typename T2, typename P>
	inline bool operator!=(STLAllocator<T,P> const&,
		STLAllocator<T2,P> const&)
	{
		// same alloc policy (P), memory can be freed
		return false;
	}

	/// determine equality, can memory from another allocator
	/// be released by this allocator, (ISO C++)
	template<typename T, typename P, typename OtherAllocator>
	inline bool operator!=(STLAllocator<T,P> const&,
		OtherAllocator const&)
	{
		return true;
	}



	//
	// Placement new/delete, with dummy enums, to differentiate from any other new/delete
	enum etOperatorPlacementNewDummy
	{
		kOperatorPlacementNewDummy
	};

	/*! Utility template function to call a destructor. */
	template<class T>
	void callDestructor(T* p)
	{
		if (p)
			p->~T();
	}

/**
	@}
*/

} // namespace cz


//
// Dummy new/delete operators
//
inline void* operator new(size_t, void* addr, const char*, int, const char*, cz::etOperatorPlacementNewDummy)
{
	return addr;
}
inline void* operator new(size_t, void* addr, cz::etOperatorPlacementNewDummy)
{
	return addr;
}
inline void operator delete( void* /*ptr*/, void*, cz::etOperatorPlacementNewDummy)
{
}
inline void operator delete( void* /*ptr*/, void*, const char*, int, const char*, cz::etOperatorPlacementNewDummy)
{
}


/*! \addtogroup Memory
	@{
*/

#ifdef __clcpp_parse__
	#define CZ_ALLOC(size, category) cz::CategoryAllocator::allocateBytes(size, NULL, 0, NULL)
	#define CZ_FREE(ptr, category) cz::CategoryAllocator::deallocateBytes(ptr)
	#define CZ_NEW new
	#define CZ_DELETE delete
	#define CZ_NEW_T(T, category) new(cz::CategoryAllocator::allocateBytes(sizeof(T), NULL, 0, NULL), cz::kOperatorPlacementNewDummy) T
	#define CZ_DELETE_T(ptr, category) if (ptr) {cz::callDestructor(ptr); cz::CategoryAllocator::deallocateBytes(ptr);}
#else

#if CZ_DEBUG

	/*! Allocate a block of raw memory, and indicates the category of usage
	@hideinitializer
	You should use CZ_FREE to free memory allocated with this.
	*/
	#define CZ_ALLOC(size, category) cz::CategoryAllocator<category>::allocateBytes(size, __FILE__, __LINE__, __FUNCTION__)

	/*! Free a block of raw memory, allocated with CZ_ALLOC
	@hideinitializer
	*/
	#define CZ_FREE(ptr, category) cz::CategoryAllocator<category>::deallocateBytes(ptr)

	/*! Allocates an object that inherits from TAllocatedObject
	 Use CZ_DELETE to delete the object
	 This can also be used to allocate an array of objects that inherit from TAllocatedObject. e.g:
	 AllocatedClass *array = CZ_NEW AllocatedClass[10];
	 CZ_DELETE [] array;
	 */
	#define CZ_NEW new(__FILE__, __LINE__, __FUNCTION__)

	/*! Deletes an object or array of objects allocated with CZ_NEW.
		If you allocated an array (e.g: SomeClass* array = CZ_NEW SomeClass[N]), don't forget to delete with
		( CZ_DELETE [] array )
	 */
	#define CZ_DELETE delete

	/*! Allocates one primitive type, or type that doesn't derive from TAllocatedObject. */
	// TODO : Test this
	#define CZ_NEW_T(T, category) new(cz::CategoryAllocator<category>::allocateBytes(sizeof(T), __FILE__, __LINE__, __FUNCTION__), cz::kOperatorPlacementNewDummy) T

	/*! Deletes an object created with CZ_NEW_T. */
	// TODO : Test this
	#define CZ_DELETE_T(ptr, category) if (ptr) { cz::callDestructor(ptr); cz::CategoryAllocator<category>::deallocateBytes(ptr); }

#else // CZ_DEBUG

	#define CZ_ALLOC(size, category) cz::CategoryAllocator<category>::allocateBytes(size, NULL, 0, NULL)
	#define CZ_FREE(ptr, category) cz::CategoryAllocator<category>::deallocateBytes(ptr)
	#define CZ_NEW new
	#define CZ_DELETE delete
	#define CZ_NEW_T(T, category) new(cz::CategoryAllocator<category>::allocateBytes(sizeof(T), NULL, 0, NULL), cz::kOperatorPlacementNewDummy) T
	#define CZ_DELETE_T(ptr, category) if (ptr) {cz::callDestructor(ptr); cz::CategoryAllocator<category>::deallocateBytes(ptr);}
#endif

#endif


/*! Based on boost intrusive_ptr.
Some unqualified functions must be available:
	void intrusive_ptr_add_ref(T* p);
	void intrusive_ptr_release(T* p);
The object is responsible for destroying itself when the reference counting reaches 0
*/
template<class T>
class intrusive_ptr
{
private:
	typedef intrusive_ptr this_type;

public:
	typedef T element_type;

	intrusive_ptr() : mPtr(0)
	{
	}

	intrusive_ptr(T* p, bool add_ref = true) : mPtr(p)
	{
		if (mPtr!=0 && add_ref) intrusive_ptr_add_ref(mPtr);
	}

	template <class U>
	intrusive_ptr( intrusive_ptr<U> const& other ) : mPtr(other.get())
	{
		if (mPtr!=0) intrusive_ptr_add_ref(mPtr);
	}

	intrusive_ptr( intrusive_ptr const & other) : mPtr(other.mPtr)
	{
		if (mPtr!=0) intrusive_ptr_add_ref(mPtr);
	}

	~intrusive_ptr()
	{
		if (mPtr!=0) intrusive_ptr_release(mPtr);
	}

	template<class U>
	intrusive_ptr& operator=( intrusive_ptr<U> const& other)
	{
		// Copy construct a temporary and swap
		this_type(other).swap(*this);
		return *this;
	}

	intrusive_ptr& operator=(T* p)
	{
		this_type(p).swap(*this);
		return *this;
	}

	void reset()
	{
		this_type().swap( *this );
	}

	void reset( T * p )
	{
		this_type(p).swap(*this);
	}

	T* get() const
	{
		return mPtr;
	}

	T& operator*() const
	{
		CZ_ASSERT(mPtr!=0);
		return *mPtr;
	}

	T* operator->() const
	{
		CZ_ASSERT(mPtr!=0);
		return mPtr;
	}

	operator bool() const
	{
		return mPtr==0 ? false : true;
	}

	void swap(intrusive_ptr& other)
	{
		T* tmp = mPtr;
		mPtr = other.mPtr;
		other.mPtr = tmp;
	}

private:
	T* mPtr;
};


/*! Based on boost scoped_ptr
	Stores a pointer to a dynamically allocated object. Object is guaranteed to be deleted, either
on destruction of the scoped_ptr, or via an explicit #reset  .

-It supplies a basic "resource acquisition is initialization" facility, without shared-ownership or transfer of ownership semantics.

- It cannot be used in containers.

- It cannot hold a pointer to a dynamically allocated array
*/
template <class T>
class scoped_ptr
{
public:
	
	typedef scoped_ptr<T> this_type;

	explicit scoped_ptr( T* p=0 ): mPtr( p )
	{
	}

	~scoped_ptr()
	{
		if (mPtr)
		{
			CZ_DELETE mPtr;
		}
	}

	void reset(T* p=0)
	{
		CZ_ASSERT( p==0 || p!=mPtr ); // Catch self-reset
		this_type(p).swap(*this);
	}

	T* release()
	{
		T* tmp = mPtr;
		mPtr = NULL;
		return tmp;
	}

	T& operator*() const
	{
		CZ_ASSERT( mPtr!=0 );
		return *mPtr;
	}

	T* operator->() const
	{
		CZ_ASSERT( mPtr!=0 );
		return mPtr;
	}

	T* get() const
	{
		return mPtr;
	}

	// implicit conversion to "bool"
	operator bool() const
	{
		return mPtr==0 ? false : true;
	}

	void swap(scoped_ptr& other)
	{
		T* tmp = other.mPtr;
		other.mPtr = mPtr;
		mPtr = tmp;
	}

private:
	T* mPtr;

	// Private and unimplemented, to avoid use
	scoped_ptr( scoped_ptr const & );
	scoped_ptr& operator=( scoped_ptr const & );
	void operator==( scoped_ptr const& ) const;
	void operator!=( scoped_ptr const& ) const;
};



	/*! Manages a chunk of raw memory with a specific category.
	You can use this when you need to allocate a block of memory with a specific memory category,
	and you need to keep track of the allocated size.
	*/
	template<class T, int MemCat_>
	class TBuffer
	{
	public:
		enum
		{
			MemCat = MemCat_
		};

		TBuffer()
			: mSize(0), mPtr(0)
		{
		}

		explicit TBuffer(int size)
			: mSize(size)
		{
			mPtr = static_cast<T*>(CZ_ALLOC(size*sizeof(T), MemCat));
		}

		~TBuffer()
		{
			deallocate();
		}

		void allocate(int size)
		{
			CZ_ASSERT(mPtr==0);
			mPtr = static_cast<T*>(CZ_ALLOC(size*sizeof(T), MemCat));
		}

		void deallocate()
		{
			if (mPtr)
			{
				CZ_FREE(mPtr, MemCat);
				mPtr = 0;
			}
		}

		const T* get() const
		{
			CZ_ASSERT(mPtr!=0);
			return mPtr;
		}

		T* release()
		{
			T* p = mPtr;
			mPtr = NULL;
			return p;
		}

		T* get()
		{
			CZ_ASSERT(mPtr!=0);
			return mPtr;
		}

		int size() const
		{
			return mSize;
		}

		operator bool() const
		{
			return mPtr==0 ? false : true;
		}

	private:
		TBuffer(TBuffer& other);
		TBuffer& operator=(TBuffer& other);

		int mSize;
		T* mPtr;
	};

/**
	@}
*/



// This include allows specialization of CategoryAllocator without needing to edit czMemory.h
#include "config/czMemoryCustom.h"


#endif // czMemory_h__
