#include "table.h"
#include <x86intrin.h>
#include <stdint.h>

tbl_hash_t tbl_hash(const tbl_key_t key)
{
	assert(key);

	uint32_t hash = 0;

	size_t i = 0;
	while(key[i])	hash = _mm_crc32_u8(hash, key[i++]);

	return hash;
}
