#include "table.h"

tbl_hash_t tbl_crc32asminline_hash(const tbl_key_t key)
{
	assert(key);

	uint32_t hash = 0;

	size_t i = 0;
	while(key[i])
	{
		asm volatile("crc32\t%1, %0" : "=r" (hash) : "r" (key[i]), "r" (hash));
		i++;
	}

	return hash;
}

tbl_hash_t tbl_const_hash(const tbl_key_t key __attribute__((unused)))
{
	return 0;
}

tbl_hash_t tbl_1a_hash(const tbl_key_t key)
{
	assert(key);

	return (uint8_t)key[0];
}

tbl_hash_t tbl_crc32_hash(const tbl_key_t key)
{
	assert(key);

	uint32_t hash = (uint32_t)-1;

	size_t i = 0;
	while(key[i])
	{
		hash ^= key[i];

		for (size_t k = 0; k < 8; k++)
		{
			if(hash & 1)	hash = (hash >> 1) ^ 0xEDB88320;
			else		hash >>= 1;
		}

		i++;
	}

	return ~hash;
}

tbl_hash_t tbl_crc32intrin_hash(const tbl_key_t key)
{
	assert(key);

	uint32_t hash = 0;

	size_t i = 0;
	while(key[i])	hash = _mm_crc32_u8(hash, key[i++]);

	return hash;
}

tbl_hash_t tbl_len_hash(const tbl_key_t key)
{
	assert(key);

	return strlen(key);
}

#define ROL64(x, s)	(uint64_t)((x << (s)) | (x >> (64-s)))
tbl_hash_t tbl_rol_hash(const tbl_key_t key) // table size 
{
	assert(key);

	tbl_hash_t hash = (uint8_t)key[0];

	size_t i = 1;
	while(key[i])
	{
		hash = ROL64(hash, 7);
		hash ^= (uint8_t)key[i];
		i++;
	}

	return hash;
}
#undef ROL64

tbl_hash_t tbl_sum_hash(const tbl_key_t key)
{
	assert(key);

	tbl_hash_t hash = 0;

	size_t i = 0;
	while(key[i])	hash += (uint8_t)key[i++];

	return hash;
}
