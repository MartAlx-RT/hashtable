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
typedef tbl_hash_t (*tbl_hash_func_t)(const tbl_key_t);

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
	tbl_hash_func_t hash_func;
	size_t size;
} tbl_t;

typedef struct tbl_test_t
{
	tbl_hash_func_t func;
	const char *name;
	const size_t TBL_SIZE;
	const size_t N_CYCLES;
} tbl_test_t;

void tbl_init(const size_t tbl_size, const size_t cell_capacity, tbl_hash_func_t hash_func, tbl_t *tbl);
void tbl_deinit(tbl_t *tbl);

void tbl_add(const tbl_key_t key, tbl_t *tbl);
void tbl_del(const tbl_key_t key, tbl_t *tbl);
tbl_data_t tbl_find(const tbl_key_t key, tbl_t *tbl);

size_t tbl_get_size(tbl_t *tbl);
double tbl_get_ldfactor(tbl_t *tbl);

//int key_cmp(const tbl_key_t key1, const tbl_key_t key2);	// declared in src/key_cmp.s
#endif /*TABLE_H*/
