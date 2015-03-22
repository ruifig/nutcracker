/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com

	created:	2012/03/10
	created:	10:3:2012   4:01
	filename: 	czFunctional.h
	
	purpose:
	Misc code similar to standard C++ <functional>
*********************************************************************/


#ifndef czFunctional_h__
#define czFunctional_h__

#include "czPlatformPrerequisites.h"
#include "czCallableTraits.h"

namespace cz
{

	template<class T>
	class reference_wrapper
	{
	public:
		typedef T type;
		explicit reference_wrapper(T& t_) : t(&t_) {}
		operator T& () const
		{
			return *t;
		}

		T& get() const
		{
			return *t;
		}
		T* get_pointer() const
		{
			return t;
		}
	private:
		T* t;
	};

	/*! Same as std::ref */
	template< class T >
	inline reference_wrapper<T> ref(T& t)
	{
		return reference_wrapper<T>(t);
	}

	/*! Same as std::cref */
	template< class T >
	inline reference_wrapper<T const> cref(const T& t)
	{
		return reference_wrapper<T const>(t);
	}

	namespace bind_helper
	{

		template<class CallableType, class TupleType, int NumArgsToUse> struct _CallWithTuple;
		template<class CallableType, class TupleType>
		struct _CallWithTuple<CallableType, TupleType, 0>
		{
			static typename CallableTraits<CallableType>::return_type call(const CallableType& callable, TupleType& tp)
			{
				return callable();
			}
		};

		template<class CallableType, class TupleType>
		struct _CallWithTuple<CallableType, TupleType, 1>
		{
			static typename CallableTraits<CallableType>::return_type call(const CallableType& callable, TupleType& tp)
			{
				return callable(tp.v0);
			}
		};

		template<class CallableType, class TupleType>
		struct _CallWithTuple<CallableType, TupleType, 2>
		{
			static typename CallableTraits<CallableType>::return_type call(const CallableType& callable, TupleType& tp)
			{
				return callable(tp.v0, tp.v1);
			}
		};

		template<class CallableType, class TupleType>
		struct _CallWithTuple<CallableType, TupleType, 3>
		{
			static typename CallableTraits<CallableType>::return_type call(const CallableType& callable, TupleType& tp)
			{
				return callable(tp.v0, tp.v1, tp.v2);
			}
		};

		template<class CallableType, class TupleType>
		struct _CallWithTuple<CallableType, TupleType, 4>
		{
			static typename CallableTraits<CallableType>::return_type call(const CallableType& callable, TupleType& tp)
			{
				return callable(tp.v0, tp.v1, tp.v2, tp.v3);
			}
		};

		template<class CallableType, class TupleType>
		struct _CallWithTuple<CallableType, TupleType, 5>
		{
			static typename CallableTraits<CallableType>::return_type call(const CallableType& callable, TupleType& tp)
			{
				return callable(tp.v0, tp.v1, tp.v2, tp.v3, tp.v4);
			}
		};

		template<class CallableType, class TupleType>
		struct _CallWithTuple<CallableType, TupleType, 6>
		{
			static typename CallableTraits<CallableType>::return_type call(const CallableType& callable, TupleType& tp)
			{
				return callable(tp.v0, tp.v1, tp.v2, tp.v3, tp.v4, tp.v5);
			}
		};

		template<class CallableType, class TupleType>
		struct _CallWithTuple<CallableType, TupleType, 7>
		{
			static typename CallableTraits<CallableType>::return_type call(const CallableType& callable, TupleType& tp)
			{
				return callable(tp.v0, tp.v1, tp.v2, tp.v3, tp.v4, tp.v5, tp.v6);
			}
		};

		template<class CallableType, class TupleType>
		struct _CallWithTuple<CallableType, TupleType, 8>
		{
			static typename CallableTraits<CallableType>::return_type call(const CallableType& callable, TupleType& tp)
			{
				return callable(tp.v0, tp.v1, tp.v2, tp.v3, tp.v4, tp.v5, tp.v6, tp.v7);
			}
		};

		template<class CallableType, class TupleType>
		struct _CallWithTuple<CallableType, TupleType, 9>
		{
			static typename CallableTraits<CallableType>::return_type call(const CallableType& callable, TupleType& tp)
			{
				return callable(tp.v0, tp.v1, tp.v2, tp.v3, tp.v4, tp.v5, tp.v6, tp.v7, tp.v8);
			}
		};

		template<class CallableType, class TupleType>
		struct _CallWithTuple<CallableType, TupleType, 10>
		{
			static typename CallableTraits<CallableType>::return_type call(const CallableType& callable, TupleType& tp)
			{
				return callable(tp.v0, tp.v1, tp.v2, tp.v3, tp.v4, tp.v5, tp.v6, tp.v7, tp.v8, tp.v9);
			}
		};

	}

	/*! Calls a callable object with the parameters specified in a tuple */
	template<class CallableType, class TupleType>
	typename CallableTraits<CallableType>::return_type callWithTuple(CallableType f, TupleType& t)
	{
		return bind_helper::_CallWithTuple<CallableType, TupleType, CallableTraits<CallableType>::NUM_ARGS>::call(f, t);
	}

	template<class F>
	class CallableBind
	{
	public:

		// Allow copying from any other CallableBind
		template<class OtherCallable>
		CallableBind( const CallableBind<OtherCallable>& other)
		{
			callable = other.callable;
			params = other.params;
		}

		// Allow copying from any other CallableBind
		template<class OtherCallable>
		CallableBind& operator=( const CallableBind<OtherCallable>& other)
		{
			callable = other.callable;
			params = other.params;
		}

		CallableBind(F& f) :
		callable(f)
		{}

		template<class A1>
		CallableBind(F f, A1& a1) :
		callable(f), params(a1)
		{}

		template<class A1, class A2>
		CallableBind(F f, A1 a1, A2 a2) :
		callable(f), params(a1, a2)
		{}

		template<class A1, class A2, class A3>
		CallableBind(F f, A1& a1, A2& a2, A3& a3) :
		callable(f), params(a1, a2, a3)
		{}

		template<class A1, class A2, class A3, class A4>
		CallableBind(F f, A1& a1, A2& a2, A3& a3, A4& a4) :
		callable(f), params(a1, a2, a3, a4)
		{}

		template<class A1, class A2, class A3, class A4, class A5>
		CallableBind(F f, A1& a1, A2& a2, A3& a3, A4& a4, A5& a5) :
		callable(f), params(a1, a2, a3, a4, a5)
		{}

		template<class A1, class A2, class A3, class A4, class A5, class A6>
		CallableBind(F f, A1& a1, A2& a2, A3& a3, A4& a4, A5& a5, A6& a6) :
		callable(f), params(a1, a2, a3, a4, a5, a6)
		{}

		template<class A1, class A2, class A3, class A4, class A5, class A6, class A7>
		CallableBind(F f, A1& a1, A2& a2, A3& a3, A4& a4, A5& a5, A6& a6, A7& a7) :
		callable(f), params(a1, a2, a3, a4, a5, a6, a7)
		{}

		template<class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
		CallableBind(F f, A1& a1, A2& a2, A3& a3, A4& a4, A5& a5, A6& a6, A7& a7, A8& a8) :
		callable(f), params(a1, a2, a3, a4, a5, a6, a7, a8)
		{}

		template<class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
		CallableBind(F f, A1& a1, A2& a2, A3& a3, A4& a4, A5& a5, A6& a6, A7& a7, A8& a8, A9& a9) :
		callable(f), params(a1, a2, a3, a4, a5, a6, a7, a8, a9)
		{}

		typename CallableTraits<F>::params_type params;

		typename CallableTraits<F>::return_type operator() ()
		{
			return callWithTuple(callable, params);
		}

		// Declare any other CallableBind classes as friends
		template <typename OtherCallable> friend class CallableBind;
	private:
		typename CallableTraits<F>::funcptr_type callable;

	};


	template<class F>
	CallableBind<F> bind(F f)
	{
		CZ_COMPILETIME_ASSERT(CallableTraits<F>::NUM_ARGS==0);
		return CallableBind<F>(f);
	}

	template<class F, class A1>
	CallableBind<F> bind(F f, A1 const& a1)
	{
		CZ_COMPILETIME_ASSERT(CallableTraits<F>::NUM_ARGS==1);
		return CallableBind<F>(f, a1);
	}

	template<class F, class A1, class A2>
	CallableBind<F> bind(F f, A1 a1, A2 a2)
	{
		CZ_COMPILETIME_ASSERT(CallableTraits<F>::NUM_ARGS==2);
		return CallableBind<F>(f, a1, a2);
	}

	template<class F, class A1, class A2, class A3>
	CallableBind<F> bind(F f, A1 a1, A2 a2, A3 a3)
	{
		CZ_COMPILETIME_ASSERT(CallableTraits<F>::NUM_ARGS==3);
		return CallableBind<F>(f, a1, a2, a3);
	}

	template<class F, class A1, class A2, class A3, class A4>
	CallableBind<F> bind(F f, A1 a1, A2 a2, A3 a3, A4 a4)
	{
		CZ_COMPILETIME_ASSERT(CallableTraits<F>::NUM_ARGS==4);
		return CallableBind<F>(f, a1, a2, a3, a4);
	}

	template<class F, class A1, class A2, class A3, class A4, class A5>
	CallableBind<F> bind(F f, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5)
	{
		CZ_COMPILETIME_ASSERT(CallableTraits<F>::NUM_ARGS==5);
		return CallableBind<F>(f, a1, a2, a3, a4, a5);
	}

	template<class F, class A1, class A2, class A3, class A4, class A5, class A6>
	CallableBind<F> bind(F f, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6)
	{
		CZ_COMPILETIME_ASSERT(CallableTraits<F>::NUM_ARGS==6);
		return CallableBind<F>(f, a1, a2, a3, a4, a5, a6);
	}

	template<class F, class A1, class A2, class A3, class A4, class A5, class A6, class A7>
	CallableBind<F> bind(F f, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7)
	{
		CZ_COMPILETIME_ASSERT(CallableTraits<F>::NUM_ARGS==7);
		return CallableBind<F>(f, a1, a2, a3, a4, a5, a6, a7);
	}

	template<class F, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
	CallableBind<F> bind(F f, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8)
	{
		CZ_COMPILETIME_ASSERT(CallableTraits<F>::NUM_ARGS==8);
		return CallableBind<F>(f, a1, a2, a3, a4, a5, a6, a7, a8);
	}

	template<class F, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
	CallableBind<F> bind(F f, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9)
	{
		CZ_COMPILETIME_ASSERT(CallableTraits<F>::NUM_ARGS==9);
		return CallableBind<F>(f, a1, a2, a3, a4, a5, a6, a7, a8, a9);
	}

} // namespace cz

#endif // czFunctional_h__
