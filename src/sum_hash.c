#include "table.h"
#include <stdint.h>

uint64_t tbl_hash(const tbl_key_t key, const uint64_t max_hash)
{
	uint64_t hash = 0;

	size_t i = 0;
	while(key[i])	hash += key[i++];

	return hash % max_hash;
}
