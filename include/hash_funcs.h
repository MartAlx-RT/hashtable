#ifndef HASH_FUNCS_H
#define HASH_FUNCS_H
#include "table.h"

tbl_hash_t tbl_crc32asminline_hash(const tbl_key_t key);
tbl_hash_t tbl_const_hash(const tbl_key_t key __attribute__((unused)));
tbl_hash_t tbl_1a_hash(const tbl_key_t key);
tbl_hash_t tbl_crc32_hash(const tbl_key_t key);
tbl_hash_t tbl_crc32intrin_hash(const tbl_key_t key);
tbl_hash_t tbl_len_hash(const tbl_key_t key);
tbl_hash_t tbl_rol_hash(const tbl_key_t key);
tbl_hash_t tbl_sum_hash(const tbl_key_t key);
tbl_hash_t tbl_crc32asm_hash(const tbl_key_t key);
tbl_hash_t tbl_crc32intrin64_hash(const tbl_key_t key);
tbl_hash_t tbl_crc32intrin64_uroll(const tbl_key_t key);

#endif /* HASH_FUNCS_H */
