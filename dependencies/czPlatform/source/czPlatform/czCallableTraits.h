/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com

	created:	2012/03/02
	created:	2:3:2012   1:33
	filename: 	czCallableTraits.h

	purpose:

*********************************************************************/


#ifndef czCallableTraits_h__
#define czCallableTraits_h__

#include "czPlatformPrerequisites.h"
#include "czTuple.h"

namespace cz
{

	template<class T>
	struct CallableTraits;

	template<>
	struct CallableTraits< void (*)(void) >
	{
		enum { NUM_ARGS=0};
		typedef void return_type;
		typedef tuple<> params_type;
		typedef void (*funcptr_type)();
	};

	template<class R, class A1>
	struct CallableTraits< R (*)(A1) >
	{
		enum { NUM_ARGS=1};
		typedef R return_type;
		typedef tuple<A1> params_type;
		typedef R (*funcptr_type)(A1);
	};

	template<class R, class A1, class A2>
	struct CallableTraits< R (*)(A1, A2) >
	{
		enum { NUM_ARGS=2};
		typedef R return_type;
		typedef tuple<A1, A2> params_type;
		typedef R (*funcptr_type)(A1, A2);
	};

	template<class R, class A1, class A2, class A3>
	struct CallableTraits< R (*)(A1, A2, A3) >
	{
		enum { NUM_ARGS=3};
		typedef R return_type;
		typedef tuple<A1, A2, A3> params_type;
		typedef R (*funcptr_type)(A1, A2, A3);
	};

	template<class R, class A1, class A2, class A3, class A4>
	struct CallableTraits< R (*)(A1, A2, A3, A4) >
	{
		enum { NUM_ARGS=4};
		typedef R return_type;
		typedef tuple<A1, A2, A3, A4> params_type;
		typedef R (*funcptr_type)(A1, A2, A3, A4);
	};

	template<class R, class A1, class A2, class A3, class A4, class A5>
	struct CallableTraits< R (*)(A1, A2, A3, A4, A5) >
	{
		enum { NUM_ARGS=5};
		typedef R return_type;
		typedef tuple<A1, A2, A3, A4, A5> params_type;
		typedef R (*funcptr_type)(A1, A2, A3, A4, A5);
	};

	template<class R, class A1, class A2, class A3, class A4, class A5, class A6>
	struct CallableTraits< R (*)(A1, A2, A3, A4, A5, A6) >
	{
		enum { NUM_ARGS=6};
		typedef R return_type;
		typedef tuple<A1, A2, A3, A4, A5, A6> params_type;
		typedef R (*funcptr_type)(A1, A2, A3, A4, A5, A6);
	};

	template<class R, class A1, class A2, class A3, class A4, class A5, class A6, class A7>
	struct CallableTraits< R (*)(A1, A2, A3, A4, A5, A6, A7) >
	{
		enum { NUM_ARGS=7};
		typedef R return_type;
		typedef tuple<A1, A2, A3, A4, A5, A6, A7> params_type;
		typedef R (*funcptr_type)(A1, A2, A3, A4, A5, A6, A7);
	};

	template<class R, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
	struct CallableTraits< R (*)(A1, A2, A3, A4, A5, A6, A7, A8) >
	{
		enum { NUM_ARGS=8};
		typedef R return_type;
		typedef tuple<A1, A2, A3, A4, A5, A6, A7, A8> params_type;
		typedef R (*funcptr_type)(A1, A2, A3, A4, A5, A6, A7, A8);
	};

	template<class R, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
	struct CallableTraits< R (*)(A1, A2, A3, A4, A5, A6, A7, A8, A9) >
	{
		enum { NUM_ARGS=9};
		typedef R return_type;
		typedef tuple<A1, A2, A3, A4, A5, A6, A7, A8, A9> params_type;
		typedef R (*funcptr_type)(A1, A2, A3, A4, A5, A6, A7, A8, A9);
	};


	template<class R, class A1>
	struct CallableTraits< R (A1) > : public CallableTraits<R (*)(A1)> { };
	template<class R, class A1, class A2>
	struct CallableTraits< R (A1, A2) > : public CallableTraits<R (*)(A1,A2)> { };
	template<class R, class A1, class A2, class A3>
	struct CallableTraits< R (A1, A2, A3) > : public CallableTraits<R (*)(A1,A2,A3)> { };
	template<class R, class A1, class A2, class A3, class A4>
	struct CallableTraits< R (A1, A2, A3, A4) > : public CallableTraits<R (*)(A1,A2,A3,A4)> { };
	template<class R, class A1, class A2, class A3, class A4, class A5>
	struct CallableTraits< R (A1, A2, A3, A4, A5) > : public CallableTraits<R (*)(A1,A2,A3,A4,A5)> { };
	template<class R, class A1, class A2, class A3, class A4, class A5, class A6>
	struct CallableTraits< R (A1, A2, A3, A4, A5, A6) > : public CallableTraits<R (*)(A1,A2,A3,A4,A5,A6)> { };
	template<class R, class A1, class A2, class A3, class A4, class A5, class A6, class A7>
	struct CallableTraits< R (A1, A2, A3, A4, A5, A6, A7) > : public CallableTraits<R (*)(A1,A2,A3,A4,A5,A6,A7)> { };
	template<class R, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
	struct CallableTraits< R (A1, A2, A3, A4, A5, A6, A7, A8) > : public CallableTraits<R (*)(A1,A2,A3,A4,A5,A6,A7,A8)> { };
	template<class R, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
	struct CallableTraits< R (A1, A2, A3, A4, A5, A6, A7, A8, A9) > : public CallableTraits<R (*)(A1,A2,A3,A4,A5,A6,A7,A8,A9)> { };



}


#endif // czCallableTraits_h__
