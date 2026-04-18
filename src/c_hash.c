#include "table.h"
#include <stdint.h>

tbl_hash_t tbl_hash(const tbl_key_t key)
{
	assert(key);

	return (uint8_t)key[0];
}
