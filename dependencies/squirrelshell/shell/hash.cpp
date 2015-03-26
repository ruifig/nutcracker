// Squirrel Shell
// Copyright (c) 2006-2008, Constantin Makshin
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#include "hash.h"

#define MAX_HASH_SIZE	 16
#define HASH_BUFFER_SIZE (MAX_HASH_SIZE * 2 + 1)

// Present hash value as string
static SQChar* HashAsString (const unsigned char* hash, unsigned len)
{
	static SQChar res[HASH_BUFFER_SIZE];
	SQChar* c = res;
	for (; len; --len, ++hash, c += 2)
	{
#if defined(_MSC_VER) && (_MSC_VER >= 1400)
		sprintf_s(c, HASH_BUFFER_SIZE, "%02x", *hash);
#else
		sprintf(c, "%02x", *hash);
#endif
	}
	*c = 0;
	return res;
}

// Calculate some hash value for file data
static SQInteger Hash (void (*func) (FILE*, unsigned char*, unsigned char*, SQInteger), unsigned hashSize)
{
	unsigned char hash[MAX_HASH_SIZE];
	memset(hash, 0, sizeof(hash));
	const SQChar* path;
	sq_getstring(sqvm, 2, &path);

	SQInteger from = 0;
	if (sq_gettop(sqvm) >= 3)
	{
		sq_getinteger(sqvm, 3, &from);
		if (from < 0)
			from = 0;
	}

	SQInteger to = -1;
	if (sq_gettop(sqvm) == 4)
	{
		sq_getinteger(sqvm, 4, &to);
		if ((to > 0) && (to <= from))
			goto finish;
	}

	FILE* file;
#if defined(_MSC_VER) && (_MSC_VER >= 1400)
	if (fopen_s(&file, path, "rb"))
		file = NULL;
#else
	file = fopen(path, "rb");
#endif
	if (!file)
		goto finish;

	fseek(file, 0, SEEK_END);
	long size;
	size = ftell(file);
	if (from >= size)
		goto finish;
	if ((to < 0) || (to > size))
		to = size;

	unsigned char block[BLOCK_SIZE];
	fseek(file, long(from), SEEK_SET);
	func(file, block, hash, to - from);
	fclose(file);

finish:
	sq_pushstring(sqvm, HashAsString(hash, hashSize), -1);
	return 1;
}

#define HASH(func, size) \
	extern void Hash_##func (FILE*, unsigned char*, unsigned char*, SQInteger); \
	static SQInteger func (HSQUIRRELVM) \
	{ \
		return Hash(Hash_##func, size); \
	}

// string md5 (string path[, integer from = 0[, integer to = -1]])
// Calculate MD5 hash for the block beginning at 'from' and ending at 'to' of the file 'path'
HASH(MD5, 16)

// string crc32 (string path[, integer from = 0[, integer to = -1]])
// Calculate CRC32 value for the block beginning at 'from' and ending at 'to' of the file 'path'
HASH(CRC32, 4)

// string adler32 (string path[, integer from = 0[, integer to = -1]])
// Calculate adler32 value for the block beginning at 'from' and ending at 'to' of the file 'path'
HASH(Adler32, 4)

void Init_Hash ()
{
	SetSqFunction("md5", MD5, -2, ". s i i");
	SetSqFunction("crc32", CRC32, -2, ". s i i");
	SetSqFunction("adler32", Adler32, -2, ". s i i");
	_RPT0(_CRT_WARN, "------ Hash library initialized\n");
}

void Shutdown_Hash ()
{
	_RPT0(_CRT_WARN, "------ Hash library deinitialized\n");
}
