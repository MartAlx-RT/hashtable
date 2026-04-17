#include "table.h"

#define ROL64(x, s)	(uint64_t)((x << (s)) | (x >> (64-s)))

tbl_hash_t tbl_hash(const tbl_key_t key) // table size 
{
	tbl_hash_t hash = key[0];

	size_t i = 1;
	while(key[i])
	{
		hash = ROL64(hash, 7);
		hash ^= key[i];
		i++;
	}

	return hash;
}
