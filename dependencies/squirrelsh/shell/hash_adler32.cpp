/* adler32.c -- compute the Adler-32 checksum of a data stream
 * Copyright (C) 1995-2004 Mark Adler
 * For conditions of distribution and use, see COPYING-zlib
 *
 * ---------------------------------------------------------------------------
 *
 * Modified by Constantin Makshin
 */

#include "hash.h"

#define BASE 65521ul
#define NMAX 5552

#define DO1(buf,i) {adler += (buf)[i]; sum2 += adler;}
#define DO2(buf,i) DO1(buf,i); DO1(buf,i+1);
#define DO4(buf,i) DO2(buf,i); DO2(buf,i+2);
#define DO8(buf,i) DO4(buf,i); DO4(buf,i+4);
#define DO16(buf)  DO8(buf,0); DO8(buf,8);
#define MOD(a)	   a %= BASE
#define MOD4(a)    a %= BASE

static unsigned adler32 (unsigned adler, const unsigned char* buf, size_t len)
{
	unsigned sum2 = (adler >> 16) & 0xffff;
	adler &= 0xffff;

	if (len == 1)
	{
		adler += buf[0];
		if (adler >= BASE)
			adler -= BASE;
		sum2 += adler;
		if (sum2 >= BASE)
			sum2 -= BASE;
		return adler | (sum2 << 16);
	}

	if (len < 16)
	{
		while (len--)
		{
			adler	+= *buf++;
			sum2	+= adler;
		}
		if (adler >= BASE)
			adler -= BASE;
		MOD4(sum2);
		return adler | (sum2 << 16);
	}

	while (len >= NMAX)
	{
		len -= NMAX;
		unsigned n = NMAX / 16;
		do
		{
			DO16(buf);
			buf += 16;
		} while (--n);
		MOD(adler);
		MOD(sum2);
	}

	if (len)
	{
		while (len >= 16)
		{
			len -= 16;
			DO16(buf);
			buf += 16;
		}
		while (len--)
		{
			adler += *buf++;
			sum2 += adler;
		}
		MOD(adler);
		MOD(sum2);
	}

	return adler | (sum2 << 16);
}

void Hash_Adler32 (FILE* file, unsigned char* block, unsigned char* hash, SQInteger left)
{
	unsigned adler = 1;
	do
	{
		size_t r = fread(block, 1, min(left, BLOCK_SIZE), file);
		adler = adler32(adler, block, r);
		left -= SQInteger(r);
	} while (left);
	memcpy(hash, &adler, 4);
}
