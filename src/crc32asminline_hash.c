#include "table.h"

tbl_hash_t tbl_hash(const tbl_key_t key)
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
