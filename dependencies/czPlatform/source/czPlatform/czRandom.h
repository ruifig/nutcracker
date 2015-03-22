/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com

	created:	2012/02/26
	created:	26:2:2012   1:39
	filename: 	czRandom.h
	
	purpose:
	
*********************************************************************/

 /** @file */

#ifndef _CZ_RANDOM_H_
#define _CZ_RANDOM_H_

#include "czPlatformPrerequisites.h"

/*
This is required. This means Mersenne exponent and the period of generated code will be 2MEXP-1.
MEXP must be one of 607, 1279, 2281, 4253, 11213, 19937, 44497, 86243, 132049, 216091.
NOTE: I'm naming this here as CZ_MT_MEXP, so I can hide the original SFMT code
*/
#define CZ_SFMT_MEXP 607

#define CZ_SFMT_N (CZ_SFMT_MEXP / 128 + 1)

namespace cz
{
	/*! \addtogroup General
		@{
	*/

	/*! Random number generator based on http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/SFMT/index.html
	*/
	class RandomGenerator
	{
	public:
		enum
		{

		};

		/*! Constructs a random number generator using the specified seed
		\param seed Seed to use. You should use a different seed every time. A good way to do that is to use
		the current time as the seed. For example, you can use the POSIX 'time' function:
		\code
		RandomGenerator rand((u32)time( NULL ) );
		\endcode
		*/
		explicit RandomGenerator(u32 seed);

		/*! Generates a random number */
		u32 gen32();

		/*! Generates a random integer in the specified range [low, high[
		\note 'high' is exclusive
		*/
		int gen32(int low, int high)
		{
			CZ_ASSERT(low < high);
			return low + gen32() % (high - low);
		}

		/*! Generates a random integer in the specified range [low, high]
		\note 'high' is inclusive
		*/
		int gen32Inclusive(int low, int high)
		{
			CZ_ASSERT(low < high);
			return low + gen32() % (high - low + 1);
		}

		/*! Generates a random double in the specified range [low, high[
		\note 'high' is exclusive
		*/
		double gen(double low, double high)
		{
			CZ_ASSERT(low < high);
			double r = double(gen32()) / (double(CZ_U32_MAX)+1);
			return low + r*(high-low);
		}

		/*! Generates a random double in the specified range [low, high]
		\note 'high' is inclusive
		*/
		double genInclusive(double low, double high)
		{
			CZ_ASSERT(low < high);
			return low + gen32() * (high - low) / CZ_U32_MAX;
		}


	private:

		/** 128-bit data structure */
		struct W128_T {
			uint32_t u[4];
		};

		/** 128-bit data type */
		typedef struct W128_T w128_t;

		static inline int idxof(int i)
		{
			return i;
		}
		
		inline static void rshift128(w128_t *out,  w128_t const *in, int shift);
		inline static void lshift128(w128_t *out,  w128_t const *in, int shift);

		void period_certification(void);
		inline void gen_rand_all(void);
		inline static void do_recursion(w128_t *r, w128_t *a, w128_t *b, w128_t *c,	w128_t *d);

		/** the 32bit integer pointer to the 128-bit internal state array */
		uint32_t *psfmt32;

		/** the 128-bit internal state array */
		w128_t sfmt[CZ_SFMT_N];

		/** index counter to the 32-bit internal state array */
		int idx;

		// Tells if the generator was initialized
		int initialized;

		static uint32_t parity[4];

	};


	/*!
		@}
	*/

} // namespace cz

#endif

