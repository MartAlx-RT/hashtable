#include "table.h"
#include <x86intrin.h>
#include <stdint.h>

tbl_hash_t tbl_hash(const tbl_key_t key)	// TODO rewrite on asm
{
	assert(key);		// TODO test unrolling

	uint32_t hash = 0;

	size_t i = 0;		// TODO use expanded crc
	while(key[i])	hash = _mm_crc32_u8(hash, key[i++]);

	return hash;
}
