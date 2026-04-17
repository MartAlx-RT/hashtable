#include "table.h"
#include <stdint.h>

tbl_hash_t tbl_hash(const tbl_key_t key)
{
	tbl_hash_t hash = 0;

	size_t i = 0;
	while(key[i])	hash += key[i++];

	return hash;
}
