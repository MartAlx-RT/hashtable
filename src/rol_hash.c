#include "table.h"
#include <stdint.h>

#define ROL64(x, s)	(uint64_t)((x << (s)) | (x >> (64-s)))

uint64_t tbl_hash(const tbl_key_t key, const uint64_t max_hash) // table size 
{
	uint64_t hash = key[0];

	size_t i = 1;
	while(key[i])
	{
		hash = ROL64(hash, 7);
		hash ^= key[i];
		i++;
	}

	return hash % max_hash;		// TODO use full hash
}
