/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com

	created:	2012/03/03
	created:	3:3:2012   15:18
	filename: 	czHelperMacros.h
	
	purpose:
	
*********************************************************************/

#ifdef _CZ_DEFINE_HELPER_MACROS

#define CZ_REP_PRE_POST_0_9(pre, T, post) pre T##0 post, pre T##1 post, pre T##2 post, pre T##3, pre T##4 post, pre T##5 post, pre T##6 post, pre T##7 post, pre T##8 post, pre T##9 post
#define CZ_REP_PRE_POST_0_8(pre, T, post) pre T##0 post, pre T##1 post, pre T##2 post, pre T##3, pre T##4 post, pre T##5 post, pre T##6 post, pre T##7 post, pre T##8 post
#define CZ_REP_PRE_POST_0_7(pre, T, post) pre T##0 post, pre T##1 post, pre T##2 post, pre T##3, pre T##4 post, pre T##5 post, pre T##6 post, pre T##7 post
#define CZ_REP_PRE_POST_0_6(pre, T, post) pre T##0 post, pre T##1 post, pre T##2 post, pre T##3, pre T##4 post, pre T##5 post, pre T##6 post
#define CZ_REP_PRE_POST_0_5(pre, T, post) pre T##0 post, pre T##1 post, pre T##2 post, pre T##3, pre T##4 post, pre T##5 post
#define CZ_REP_PRE_POST_0_4(pre, T, post) pre T##0 post, pre T##1 post, pre T##2 post, pre T##3, pre T##4 post
#define CZ_REP_PRE_POST_0_3(pre, T, post) pre T##0 post, pre T##1 post, pre T##2 post, pre T##3
#define CZ_REP_PRE_POST_0_2(pre, T, post) pre T##0 post, pre T##1 post, pre T##2 post
#define CZ_REP_PRE_POST_0_1(pre, T, post) pre T##0 post, pre T##1 post
#define CZ_REP_PRE_POST_0_0(pre, T, post) pre T##0 post

#define CZ_REP_PRE_0_9(pre, T) pre T##0, pre T##1, pre T##2, pre T##3, pre T##4, pre T##5, pre T##6, pre T##7, pre T##8, pre T##9
#define CZ_REP_PRE_0_8(pre, T) pre T##0, pre T##1, pre T##2, pre T##3, pre T##4, pre T##5, pre T##6, pre T##7, pre T##8
#define CZ_REP_PRE_0_7(pre, T) pre T##0, pre T##1, pre T##2, pre T##3, pre T##4, pre T##5, pre T##6, pre T##7
#define CZ_REP_PRE_0_6(pre, T) pre T##0, pre T##1, pre T##2, pre T##3, pre T##4, pre T##5, pre T##6
#define CZ_REP_PRE_0_5(pre, T) pre T##0, pre T##1, pre T##2, pre T##3, pre T##4, pre T##5
#define CZ_REP_PRE_0_4(pre, T) pre T##0, pre T##1, pre T##2, pre T##3, pre T##4
#define CZ_REP_PRE_0_3(pre, T) pre T##0, pre T##1, pre T##2, pre T##3
#define CZ_REP_PRE_0_2(pre, T) pre T##0, pre T##1, pre T##2
#define CZ_REP_PRE_0_1(pre, T) pre T##0, pre T##1
#define CZ_REP_PRE_0_0(pre, T) pre T##0

#define CZ_REP_0_9(T) T##0, T##1, T##2, T##3, T##4, T##5, T##6, T##7, T##8, T##9
#define CZ_REP_0_8(T) T##0, T##1, T##2, T##3, T##4, T##5, T##6, T##7, T##8
#define CZ_REP_0_7(T) T##0, T##1, T##2, T##3, T##4, T##5, T##6, T##7
#define CZ_REP_0_6(T) T##0, T##1, T##2, T##3, T##4, T##5, T##6
#define CZ_REP_0_5(T) T##0, T##1, T##2, T##3, T##4, T##5
#define CZ_REP_0_4(T) T##0, T##1, T##2, T##3, T##4
#define CZ_REP_0_3(T) T##0, T##1, T##2, T##3
#define CZ_REP_0_2(T) T##0, T##1, T##2
#define CZ_REP_0_1(T) T##0, T##1
#define CZ_REP_0_0(T) T##0

#else

#undef CZ_REP_PRE_POST_0_9
#undef CZ_REP_PRE_POST_0_8
#undef CZ_REP_PRE_POST_0_7
#undef CZ_REP_PRE_POST_0_6
#undef CZ_REP_PRE_POST_0_5
#undef CZ_REP_PRE_POST_0_4
#undef CZ_REP_PRE_POST_0_3
#undef CZ_REP_PRE_POST_0_2
#undef CZ_REP_PRE_POST_0_1
#undef CZ_REP_PRE_POST_0_0

#undef CZ_REP_PRE_0_9
#undef CZ_REP_PRE_0_8
#undef CZ_REP_PRE_0_7
#undef CZ_REP_PRE_0_6
#undef CZ_REP_PRE_0_5
#undef CZ_REP_PRE_0_4
#undef CZ_REP_PRE_0_3
#undef CZ_REP_PRE_0_2
#undef CZ_REP_PRE_0_1
#undef CZ_REP_PRE_0_0

#undef CZ_REP_0_9
#undef CZ_REP_0_8
#undef CZ_REP_0_7
#undef CZ_REP_0_6
#undef CZ_REP_0_5
#undef CZ_REP_0_4
#undef CZ_REP_0_3
#undef CZ_REP_0_2
#undef CZ_REP_0_1
#undef CZ_REP_0_0

#endif

#define CZ_EXP_EQUALSVOID =void
