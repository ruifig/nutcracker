/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com

	created:	2012/03/02
	created:	2:3:2012   0:23
	filename: 	czTuple.h
	
	purpose:
	somewhat similar to boost/std tuple
*********************************************************************/


#ifndef czTuple_h__
#define czTuple_h__

#include "czPlatformPrerequisites.h"
#include "czTypeTraits.h"

namespace cz
{

	// Copied from boost
	// -----------------
	template <class T> struct access_traits
	{
		typedef const T& const_type;
		typedef T& non_const_type;

		typedef const typename ::cz::remove_cv< T >::type& parameter_type;
		// used as the tuple constructors parameter types
		// Rationale: non-reference tuple element types can be cv-qualified.
		// It should be possible to initialize such types with temporaries,
		// and when binding temporaries to references, the reference must
		// be non-volatile and const. 8.5.3. (5)
	};

	template <class T> struct access_traits<T&> {

		typedef T& const_type;
		typedef T& non_const_type;

		typedef T& parameter_type;
	};
	// -----------------

	template<class T0=void, class T1=void, class T2=void, class T3=void, class T4=void, class T5=void, class T6=void, class T7=void, class T8=void, class T9=void>
	struct tuple
	{
		tuple(){}
		tuple(
			typename access_traits<T0>::parameter_type a0,
			typename access_traits<T1>::parameter_type a1,
			typename access_traits<T2>::parameter_type a2,
			typename access_traits<T3>::parameter_type a3,
			typename access_traits<T4>::parameter_type a4,
			typename access_traits<T5>::parameter_type a5,
			typename access_traits<T6>::parameter_type a6,
			typename access_traits<T7>::parameter_type a7,
			typename access_traits<T8>::parameter_type a8,
			typename access_traits<T9>::parameter_type a9
			) :
			v0(a0),
			v1(a1),
			v2(a2),
			v3(a3),
			v4(a4),
			v5(a5),
			v6(a6),
			v7(a7),
			v8(a8),
			v9(a9)
			{}

		template<class Y0, class Y1, class Y2, class Y3, class Y4, class Y5, class Y6, class Y7, class Y8, class Y9>
		tuple& operator=(const tuple<Y0,Y1,Y2,Y3,Y4,Y5,Y6,Y7,Y8,Y9>& other)
		{
			v0 = other.v0;
			v1 = other.v1;
			v2 = other.v2;
			v3 = other.v3;
			v4 = other.v4;
			v5 = other.v5;
			v6 = other.v6;
			v7 = other.v7;
			v8 = other.v8;
			v9 = other.v9;
			return *this;
		}

		typedef T0 T0_;
		typedef T1 T1_;
		typedef T2 T2_;
		typedef T3 T3_;
		typedef T4 T4_;
		typedef T5 T5_;
		typedef T6 T6_;
		typedef T7 T7_;
		typedef T8 T8_;
		typedef T9 T9_;
		T0 v0;
		T1 v1;
		T2 v2;
		T3 v3;
		T4 v4;
		T5 v5;
		T6 v6;
		T7 v7;
		T8 v8;
		T9 v9;
	};

	template<class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
	struct tuple<T0,T1,T2,T3,T4,T5,T6,T7,T8,void>
	{
		tuple(){}
		tuple(
			typename access_traits<T0>::parameter_type a0,
			typename access_traits<T1>::parameter_type a1,
			typename access_traits<T2>::parameter_type a2,
			typename access_traits<T3>::parameter_type a3,
			typename access_traits<T4>::parameter_type a4,
			typename access_traits<T5>::parameter_type a5,
			typename access_traits<T6>::parameter_type a6,
			typename access_traits<T7>::parameter_type a7,
			typename access_traits<T8>::parameter_type a8
			) :
			v0(a0),
			v1(a1),
			v2(a2),
			v3(a3),
			v4(a4),
			v5(a5),
			v6(a6),
			v7(a7),
			v8(a8)
		{}

		template<class Y0, class Y1, class Y2, class Y3, class Y4, class Y5, class Y6, class Y7, class Y8>
		tuple& operator=(const tuple<Y0,Y1,Y2,Y3,Y4,Y5,Y6,Y7,Y8>& other)
		{
			v0 = other.v0;
			v1 = other.v1;
			v2 = other.v2;
			v3 = other.v3;
			v4 = other.v4;
			v5 = other.v5;
			v6 = other.v6;
			v7 = other.v7;
			v8 = other.v8;
			return *this;
		}

		typedef T0 T0_;
		typedef T1 T1_;
		typedef T2 T2_;
		typedef T3 T3_;
		typedef T4 T4_;
		typedef T5 T5_;
		typedef T6 T6_;
		typedef T7 T7_;
		typedef T8 T8_;
		T0 v0;
		T1 v1;
		T2 v2;
		T3 v3;
		T4 v4;
		T5 v5;
		T6 v6;
		T7 v7;
		T8 v8;
	};

	template<class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7>
	struct tuple<T0,T1,T2,T3,T4,T5,T6,T7,void,void>
	{
		tuple(){}
		tuple(
			typename access_traits<T0>::parameter_type a0,
			typename access_traits<T1>::parameter_type a1,
			typename access_traits<T2>::parameter_type a2,
			typename access_traits<T3>::parameter_type a3,
			typename access_traits<T4>::parameter_type a4,
			typename access_traits<T5>::parameter_type a5,
			typename access_traits<T6>::parameter_type a6,
			typename access_traits<T7>::parameter_type a7
			) :
			v0(a0),
			v1(a1),
			v2(a2),
			v3(a3),
			v4(a4),
			v5(a5),
			v6(a6),
			v7(a7)
		{}

		template<class Y0, class Y1, class Y2, class Y3, class Y4, class Y5, class Y6, class Y7>
		tuple& operator=(const tuple<Y0,Y1,Y2,Y3,Y4,Y5,Y6,Y7>& other)
		{
			v0 = other.v0;
			v1 = other.v1;
			v2 = other.v2;
			v3 = other.v3;
			v4 = other.v4;
			v5 = other.v5;
			v6 = other.v6;
			v7 = other.v7;
			return *this;
		}

		typedef T0 T0_;
		typedef T1 T1_;
		typedef T2 T2_;
		typedef T3 T3_;
		typedef T4 T4_;
		typedef T5 T5_;
		typedef T6 T6_;
		typedef T7 T7_;
		T0 v0;
		T1 v1;
		T2 v2;
		T3 v3;
		T4 v4;
		T5 v5;
		T6 v6;
		T7 v7;
	};

	template<class T0, class T1, class T2, class T3, class T4, class T5, class T6>
	struct tuple<T0,T1,T2,T3,T4,T5,T6,void,void,void>
	{
		tuple(){}
		tuple(
			typename access_traits<T0>::parameter_type a0,
			typename access_traits<T1>::parameter_type a1,
			typename access_traits<T2>::parameter_type a2,
			typename access_traits<T3>::parameter_type a3,
			typename access_traits<T4>::parameter_type a4,
			typename access_traits<T5>::parameter_type a5,
			typename access_traits<T6>::parameter_type a6
			) :
			v0(a0),
			v1(a1),
			v2(a2),
			v3(a3),
			v4(a4),
			v5(a5),
			v6(a6)
		{}

		template<class Y0, class Y1, class Y2, class Y3, class Y4, class Y5, class Y6>
		tuple& operator=(const tuple<Y0,Y1,Y2,Y3,Y4,Y5,Y6>& other)
		{
			v0 = other.v0;
			v1 = other.v1;
			v2 = other.v2;
			v3 = other.v3;
			v4 = other.v4;
			v5 = other.v5;
			v6 = other.v6;
			return *this;
		}

		typedef T0 T0_;
		typedef T1 T1_;
		typedef T2 T2_;
		typedef T3 T3_;
		typedef T4 T4_;
		typedef T5 T5_;
		typedef T6 T6_;
		T0 v0;
		T1 v1;
		T2 v2;
		T3 v3;
		T4 v4;
		T5 v5;
		T6 v6;
	};

	template<class T0, class T1, class T2, class T3, class T4, class T5>
	struct tuple<T0,T1,T2,T3,T4,T5,void,void,void,void>
	{
		tuple(){}
		tuple(
			typename access_traits<T0>::parameter_type a0,
			typename access_traits<T1>::parameter_type a1,
			typename access_traits<T2>::parameter_type a2,
			typename access_traits<T3>::parameter_type a3,
			typename access_traits<T4>::parameter_type a4,
			typename access_traits<T5>::parameter_type a5
			) :
			v0(a0),
			v1(a1),
			v2(a2),
			v3(a3),
			v4(a4),
			v5(a5)
		{}

		template<class Y0, class Y1, class Y2, class Y3, class Y4, class Y5>
		tuple& operator=(const tuple<Y0,Y1,Y2,Y3,Y4,Y5>& other)
		{
			v0 = other.v0;
			v1 = other.v1;
			v2 = other.v2;
			v3 = other.v3;
			v4 = other.v4;
			v5 = other.v5;
			return *this;
		}

		typedef T0 T0_;
		typedef T1 T1_;
		typedef T2 T2_;
		typedef T3 T3_;
		typedef T4 T4_;
		typedef T5 T5_;
		T0 v0;
		T1 v1;
		T2 v2;
		T3 v3;
		T4 v4;
		T5 v5;
	};

	template<class T0, class T1, class T2, class T3, class T4>
	struct tuple<T0,T1,T2,T3,T4,void,void,void,void,void>
	{
		tuple(){}
		tuple(
			typename access_traits<T0>::parameter_type a0,
			typename access_traits<T1>::parameter_type a1,
			typename access_traits<T2>::parameter_type a2,
			typename access_traits<T3>::parameter_type a3,
			typename access_traits<T4>::parameter_type a4
			) :
			v0(a0),
			v1(a1),
			v2(a2),
			v3(a3),
			v4(a4)
		{}

		template<class Y0, class Y1, class Y2, class Y3, class Y4>
		tuple& operator=(const tuple<Y0,Y1,Y2,Y3,Y4>& other)
		{
			v0 = other.v0;
			v1 = other.v1;
			v2 = other.v2;
			v3 = other.v3;
			v4 = other.v4;
			return *this;
		}

		typedef T0 T0_;
		typedef T1 T1_;
		typedef T2 T2_;
		typedef T3 T3_;
		typedef T4 T4_;
		T0 v0;
		T1 v1;
		T2 v2;
		T3 v3;
		T4 v4;
	};

	template<class T0, class T1, class T2, class T3>
	struct tuple<T0,T1,T2,T3,void,void,void,void,void,void>
	{
		tuple(){}
		tuple(
			typename access_traits<T0>::parameter_type a0,
			typename access_traits<T1>::parameter_type a1,
			typename access_traits<T2>::parameter_type a2,
			typename access_traits<T3>::parameter_type a3
			) :
			v0(a0),
			v1(a1),
			v2(a2),
			v3(a3)
		{}

		template<class Y0, class Y1, class Y2, class Y3>
		tuple& operator=(const tuple<Y0,Y1,Y2,Y3>& other)
		{
			v0 = other.v0;
			v1 = other.v1;
			v2 = other.v2;
			v3 = other.v3;
			return *this;
		}

		typedef T0 T0_;
		typedef T1 T1_;
		typedef T2 T2_;
		typedef T3 T3_;
		T0 v0;
		T1 v1;
		T2 v2;
		T3 v3;
	};

	template<class T0, class T1, class T2>
	struct tuple<T0,T1,T2,void,void,void,void,void,void,void>
	{
		tuple(){}
		tuple(
			typename access_traits<T0>::parameter_type a0,
			typename access_traits<T1>::parameter_type a1,
			typename access_traits<T2>::parameter_type a2
			) :
			v0(a0),
			v1(a1),
			v2(a2)
		{}

		template<class Y0, class Y1, class Y2>
		tuple& operator=(const tuple<Y0,Y1,Y2>& other)
		{
			v0 = other.v0;
			v1 = other.v1;
			v2 = other.v2;
			return *this;
		}

		typedef T0 T0_;
		typedef T1 T1_;
		typedef T2 T2_;
		T0 v0;
		T1 v1;
		T2 v2;
	};

	template<class T0, class T1>
	struct tuple<T0,T1,void,void,void,void,void,void,void,void>
	{
		tuple(){}
		tuple(
			typename access_traits<T0>::parameter_type a0,
			typename access_traits<T1>::parameter_type a1
			) :
			v0(a0),
			v1(a1)
		{}

		template<class Y0, class Y1>
		tuple& operator=(const tuple<Y0,Y1>& other)
		{
			v0 = other.v0;
			v1 = other.v1;
			return *this;
		}

		typedef T0 T0_;
		typedef T1 T1_;
		T0 v0;
		T1 v1;
	};


	template<class T0>
	struct tuple<T0,void,void,void,void,void,void,void,void,void>
	{
		tuple(){}
		tuple(
			typename access_traits<T0>::parameter_type a0
			) :
			v0(a0)
		{}

		template<class Y0>
		tuple& operator=(const tuple<Y0>& other)
		{
			v0 = other.v0;
			return *this;
		}

		typedef T0 T0_;
		T0 v0;
	};

	template<>
	struct tuple<void,void,void,void,void,void,void,void,void,void>
	{
	};

	template<int IDX, class TupleClass>
	struct TupleElement;

#define CZ_TUPLE_GENERATE_ELEMENT_TYPE(IDX) \
	template<class TupleClass> \
	struct TupleElement<IDX,TupleClass> \
	{ \
		typedef typename TupleClass::T##IDX##_ Type; \
		static typename access_traits<Type>::non_const_type getVal(TupleClass& t) { return t.v##IDX;	} \
	}; \
	template<class TupleClass> \
	struct TupleElement<IDX, const TupleClass> \
	{ \
	typedef typename TupleClass::T##IDX##_ Type; \
	static typename access_traits<Type>::const_type getVal(const TupleClass& t) { return t.v##IDX;	} \
	};

	CZ_TUPLE_GENERATE_ELEMENT_TYPE(0)
	CZ_TUPLE_GENERATE_ELEMENT_TYPE(1)
	CZ_TUPLE_GENERATE_ELEMENT_TYPE(2)
	CZ_TUPLE_GENERATE_ELEMENT_TYPE(3)
	CZ_TUPLE_GENERATE_ELEMENT_TYPE(4)
	CZ_TUPLE_GENERATE_ELEMENT_TYPE(5)
	CZ_TUPLE_GENERATE_ELEMENT_TYPE(6)
	CZ_TUPLE_GENERATE_ELEMENT_TYPE(7)
	CZ_TUPLE_GENERATE_ELEMENT_TYPE(8)
	CZ_TUPLE_GENERATE_ELEMENT_TYPE(9)

	// For non-const tuples
	template<int IDX, class TupleClass>
	typename access_traits< typename TupleElement<IDX,TupleClass>::Type >::non_const_type get(TupleClass& t)
	{
		return TupleElement<IDX,TupleClass>::getVal(t);
	}

	// For const tuples
	template<int IDX, class TupleClass>
	typename access_traits< typename TupleElement<IDX, TupleClass>::Type >::const_type get(const TupleClass& t)
	{
		return TupleElement<IDX, TupleClass>::getVal(t);
	}



	template<class T0>
	tuple<T0> make_tuple(const T0& a0)
	{ return tuple<T0>(a0); }

	template<class T0, class T1>
	tuple<T0,T1> make_tuple(const T0& a0, const T1& a1)
	{ return tuple<T0,T1>(a0,a1); }

	template<class T0, class T1, class T2>
	tuple<T0,T1,T2> make_tuple(const T0& a0, const T1& a1, const T2& a2)
	{ return tuple<T0,T1,T2>(a0,a1,a2); }

} // namespace cz

#endif // czTuple_h__
