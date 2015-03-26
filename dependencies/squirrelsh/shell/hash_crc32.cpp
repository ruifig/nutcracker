/* crc32.c -- compute the CRC-32 of a data stream
 * Copyright (C) 1995-2005 Mark Adler
 * For conditions of distribution and use, see COPYING-zlib
 *
 * ---------------------------------------------------------------------------
 *
 * Modified by Constantin Makshin
 */

#include "hash.h"

static unsigned crc_table[256];

// Generate tables for a byte-wise 32-bit CRC calculation
static void make_crc_table()
{
	size_t n, k;
	static const unsigned char p[] = {0, 1, 2, 4, 5, 7, 8, 10, 11, 12, 16, 22, 23, 26};

	unsigned poly = 0u;
	for (n = 0; n < sizeof(p) / sizeof(unsigned char); n++)
		poly |= 1u << (31 - p[n]);

	for (n = 0; n < 256; n++)
	{
		unsigned c = unsigned(n);
		for (k = 0; k < 8; k++)
			c = c & 1 ? poly ^ (c >> 1) : c >> 1;
		crc_table[n] = c;
	}
}

#define	DO1	crc = crc_table[(crc ^ (*buf++)) & 0xff] ^ (crc >> 8)
#define	DO8	DO1; DO1; DO1; DO1; DO1; DO1; DO1; DO1

static unsigned crc32 (unsigned crc, const unsigned char* buf, size_t len)
{
	crc = crc ^ 0xffffffffu;
	while (len >= 8)
	{
		DO8;
		len -= 8;
	}
	if (len)
	{
		do
		{
			DO1;
		} while (--len);
	}
	return crc ^ 0xffffffffu;
}

void Hash_CRC32 (FILE* file, unsigned char* block, unsigned char* hash, SQInteger left)
{
	static bool first = true;
	if (first)
	{
		make_crc_table();
		first = false;
	}

	unsigned crc = 0;
	do
	{
		size_t r = fread(block, 1, min(left, BLOCK_SIZE), file);
		crc = crc32(crc, block, r);
		left -= SQInteger(r);
	} while (left);
	memcpy(hash, &crc, 4);
}
