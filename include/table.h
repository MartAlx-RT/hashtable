#ifndef TABLE_H
#define TABLE_H

#include <stddef.h>
#include <stdio.h>
#include <malloc.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <x86intrin.h>
#include <immintrin.h>
#include <errno.h>

#define MAX_S_LEN 64
static const uint64_t UNDEF = (uint64_t)-1; 

typedef size_t tbl_data_t;
typedef char tbl_key_t[MAX_S_LEN];
typedef uint64_t tbl_hash_t;

typedef struct tbl_cell_t
{
	tbl_key_t *keys;
	tbl_hash_t *hashes;	// full hashes

	tbl_data_t *data;
	uint64_t *next;
	uint64_t tail;		// head always = 0
	uint64_t free;
	size_t size, capacity;
} tbl_cell_t;

typedef struct tbl_t
{
	tbl_cell_t *cells;
	size_t size;
} tbl_t;

void tbl_init(const size_t tbl_size, const size_t cell_capacity, tbl_t *tbl);
void tbl_add(const tbl_key_t key, tbl_t *tbl, tbl_hash_t (*tbl_hash)(const tbl_key_t));
void tbl_deinit(tbl_t *tbl);
void tbl_del(const tbl_key_t key, tbl_t *tbl, tbl_hash_t (*tbl_hash)(const tbl_key_t));
tbl_data_t tbl_find(const tbl_key_t key, tbl_t *tbl, tbl_hash_t (*tbl_hash)(const tbl_key_t));
size_t tbl_get_size(tbl_t *tbl);
double tbl_get_ldfactor(tbl_t *tbl);
void cell_dump(tbl_cell_t *cell, FILE *f);
void tbl_dump(tbl_t *tbl, FILE *f);

/* hash functions */
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

//int key_cmp(const tbl_key_t key1, const tbl_key_t key2);	// declared in src/key_cmp.s
#endif /*TABLE_H*/
