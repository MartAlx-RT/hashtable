#include "table.h"
#include <stdint.h>

tbl_hash_t tbl_hash(const tbl_key_t key)
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
