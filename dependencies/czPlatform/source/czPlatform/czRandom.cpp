//
// CrazyGaze (http://www.crazygaze.com)
// Author : Rui Figueira
// Email  : rui@crazygaze.com
//
// --------------------------------------------------------------
//

#include "czPlatformStableHeaders.h"

#include "czRandom.h"

// Define SFMT's MEXP to our desired MEXP value
#define MEXP CZ_SFMT_MEXP
#include "detail/SFMT/SFMT-params.h"

namespace cz
{

uint32_t RandomGenerator::parity[4] = {PARITY1, PARITY2, PARITY3, PARITY4};

RandomGenerator::RandomGenerator(u32 seed)
{
	// Make sure our defines have the same value was the ones internally defined by SFMT
	CZ_COMPILETIME_ASSERT(CZ_SFMT_N==N);

	psfmt32 = &sfmt[0].u[0];

	int i;

	psfmt32[idxof(0)] = seed;
	for (i = 1; i < N32; i++) {
		psfmt32[idxof(i)] = 1812433253UL * (psfmt32[idxof(i - 1)] 
		^ (psfmt32[idxof(i - 1)] >> 30))
			+ i;
	}
	idx = N32;
	period_certification();
	initialized = 1;

}

u32 RandomGenerator::gen32()
{
	uint32_t r;

	CZ_ASSERT(initialized);
	if (idx >= N32) {
		gen_rand_all();
		idx = 0;
	}
	r = psfmt32[idx++];
	return r;
}

/**
 * This function certificate the period of 2^{MEXP}
 */
void RandomGenerator::period_certification(void) {
    int inner = 0;
    int i, j;
    uint32_t work;

    for (i = 0; i < 4; i++)
	inner ^= psfmt32[idxof(i)] & parity[i];
    for (i = 16; i > 0; i >>= 1)
	inner ^= inner >> i;
    inner &= 1;
    /* check OK */
    if (inner == 1) {
	return;
    }
    /* check NG, and modification */
    for (i = 0; i < 4; i++) {
	work = 1;
	for (j = 0; j < 32; j++) {
	    if ((work & parity[i]) != 0) {
		psfmt32[idxof(i)] ^= work;
		return;
	    }
	    work = work << 1;
	}
    }
}


/**
 * This function simulates SIMD 128-bit right shift by the standard C.
 * The 128-bit integer given in in is shifted by (shift * 8) bits.
 * This function simulates the LITTLE ENDIAN SIMD.
 * @param out the output of this function
 * @param in the 128-bit data to be shifted
 * @param shift the shift value
 */
void RandomGenerator::rshift128(w128_t *out, w128_t const *in, int shift) {
	uint64_t th, tl, oh, ol;

	th = ((uint64_t)in->u[3] << 32) | ((uint64_t)in->u[2]);
	tl = ((uint64_t)in->u[1] << 32) | ((uint64_t)in->u[0]);

	oh = th >> (shift * 8);
	ol = tl >> (shift * 8);
	ol |= th << (64 - shift * 8);
	out->u[1] = (uint32_t)(ol >> 32);
	out->u[0] = (uint32_t)ol;
	out->u[3] = (uint32_t)(oh >> 32);
	out->u[2] = (uint32_t)oh;
}

/**
 * This function simulates SIMD 128-bit left shift by the standard C.
 * The 128-bit integer given in in is shifted by (shift * 8) bits.
 * This function simulates the LITTLE ENDIAN SIMD.
 * @param out the output of this function
 * @param in the 128-bit data to be shifted
 * @param shift the shift value
 */
void RandomGenerator::lshift128(w128_t *out, w128_t const *in, int shift) {
	uint64_t th, tl, oh, ol;

	th = ((uint64_t)in->u[3] << 32) | ((uint64_t)in->u[2]);
	tl = ((uint64_t)in->u[1] << 32) | ((uint64_t)in->u[0]);

	oh = th << (shift * 8);
	ol = tl << (shift * 8);
	oh |= tl >> (64 - shift * 8);
	out->u[1] = (uint32_t)(ol >> 32);
	out->u[0] = (uint32_t)ol;
	out->u[3] = (uint32_t)(oh >> 32);
	out->u[2] = (uint32_t)oh;
}

void RandomGenerator::do_recursion(w128_t *r, w128_t *a, w128_t *b, w128_t *c, w128_t *d) {
		w128_t x;
		w128_t y;

		lshift128(&x, a, SL2);
		rshift128(&y, c, SR2);
		r->u[0] = a->u[0] ^ x.u[0] ^ ((b->u[0] >> SR1) & MSK1) ^ y.u[0] 
		^ (d->u[0] << SL1);
		r->u[1] = a->u[1] ^ x.u[1] ^ ((b->u[1] >> SR1) & MSK2) ^ y.u[1] 
		^ (d->u[1] << SL1);
		r->u[2] = a->u[2] ^ x.u[2] ^ ((b->u[2] >> SR1) & MSK3) ^ y.u[2] 
		^ (d->u[2] << SL1);
		r->u[3] = a->u[3] ^ x.u[3] ^ ((b->u[3] >> SR1) & MSK4) ^ y.u[3] 
		^ (d->u[3] << SL1);
}

/**
 * This function fills the internal state array with pseudorandom
 * integers.
 */
void RandomGenerator::gen_rand_all(void) {
    int i;
    w128_t *r1, *r2;

    r1 = &sfmt[N - 2];
    r2 = &sfmt[N - 1];
    for (i = 0; i < N - POS1; i++) {
	do_recursion(&sfmt[i], &sfmt[i], &sfmt[i + POS1], r1, r2);
	r1 = r2;
	r2 = &sfmt[i];
    }
    for (; i < N; i++) {
	do_recursion(&sfmt[i], &sfmt[i], &sfmt[i + POS1 - N], r1, r2);
	r1 = r2;
	r2 = &sfmt[i];
    }
}


}
