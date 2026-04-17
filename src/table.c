#include "table.h"
#include <asm-generic/errno.h>
#include <errno.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

void tbl_init(tbl_t *tbl, const size_t cell_capacity)
{
	assert(tbl);

	tbl->cells = (tbl_cell_t *)calloc(tbl->size, sizeof(tbl_cell_t));
	assert(tbl->cells);

	for(size_t i=0; i<tbl->size; i++)
		cell_init(&(tbl->cells[i]), cell_capacity);
}

void cell_init(tbl_cell_t *cell, const size_t cell_capacity)
{
	assert(cell);

	cell->keys = (tbl_key_t *)calloc(cell_capacity+1, sizeof(tbl_key_t));
	cell->hashes = (tbl_hash_t *)calloc(cell_capacity+1, sizeof(tbl_hash_t));
	cell->data = (tbl_data_t *)calloc(cell_capacity+1, sizeof(tbl_data_t));
	cell->next = (uint64_t *)calloc(cell_capacity+1, sizeof(uint64_t));
	assert(cell->keys);	assert(cell->hashes);
	assert(cell->next);	assert(cell->data);

	cell->next[0] = 0;
	for(size_t i = 1; i <= cell_capacity; i++)
		cell->next[i] = i+1;

	cell->tail = 0;
	cell->free = 1;
	cell->size = 0;
	cell->capacity = cell_capacity;
}

void cell_realloc(tbl_cell_t *cell)
{
	assert(cell);

	cell->capacity = 2*cell->size;
	cell->keys = (tbl_key_t *)reallocarray(cell->keys, cell->capacity+1, sizeof(tbl_key_t));
	cell->hashes = (tbl_hash_t *)reallocarray(cell->hashes, cell->capacity+1, sizeof(tbl_hash_t));
	cell->data = (tbl_data_t *)reallocarray(cell->data, cell->capacity+1, sizeof(tbl_data_t));
	cell->next = (uint64_t *)reallocarray(cell->next, cell->capacity+1, sizeof(uint64_t));
	assert(cell->keys);	assert(cell->hashes);
	assert(cell->next);	assert(cell->data);

	memset(cell->keys+cell->size, 0, sizeof(tbl_key_t)*(cell->capacity-cell->size+1));
	memset(cell->hashes+cell->size, 0, sizeof(tbl_hash_t)*(cell->capacity-cell->size+1));
	memset(cell->data+cell->size, 0, sizeof(tbl_data_t)*(cell->capacity-cell->size+1));

	for(size_t i=cell->size+1; i<=cell->capacity; i++)		// +1 ??
		cell->next[i] = i+1;
}

void tbl_add(const tbl_key_t key, tbl_t *tbl)
{
	assert(tbl);

	tbl_hash_t hash = tbl_hash(key);
	//fprintf(stderr, "%lu\n", hash % tbl->size);
	tbl_cell_t *cell = &(tbl->cells[hash % tbl->size]);
	uint64_t added = 0;

	// if element exists
	do
	{
		added = cell->next[added];

		if(cell->hashes[added] == hash && !strcmp(cell->keys[added], key))
		{
			cell->data[added]++;
			return;
		}
	}
	while(added != cell->tail);

	// if element doesn't exist

	if(++cell->size >= cell->capacity)	cell_realloc(cell);

	added = cell->free;

	strncpy(cell->keys[added], key, MAX_S_LEN);
	cell->hashes[added] = hash;

	cell->data[added] = 1;

	cell->next[cell->tail] = added;
	cell->tail = added;
	cell->free = cell->next[added];
}

void tbl_del(const tbl_key_t key, tbl_t *tbl)
{
	assert(tbl);

	tbl_hash_t hash = tbl_hash(key);
	tbl_cell_t *cell = &(tbl->cells[hash % tbl->size]);
	uint64_t deleted = 0, previous = 0;

	do
	{
		previous = deleted;
		deleted = cell->next[deleted];

		if(cell->hashes[deleted] == hash && !strcmp(cell->keys[deleted], key))
		{
			cell->next[previous] = cell->next[deleted];
			cell->next[deleted] = cell->free;
			cell->free = deleted;

			return;
		}
	}
	while(deleted != cell->tail);

	errno = ENOKEY;	return;
}

tbl_data_t tbl_find(const tbl_key_t key, tbl_t *tbl)
{
	assert(tbl);

	tbl_hash_t hash = tbl_hash(key);
	tbl_cell_t *cell = &(tbl->cells[hash % tbl->size]);
	uint64_t idx = 0;

	do
	{
		idx = cell->next[idx];
		if(cell->hashes[idx] == hash && !strcmp(cell->keys[idx], key))
			return cell->data[idx];
	}
	while(idx != cell->tail);

	errno = ENOKEY;
	return UNDEF;
}

void tbl_deinit(tbl_t *tbl)
{
	if(tbl == NULL)	return;

	for(size_t i = 0; i < tbl->size; i++)
	{
		free(tbl->cells[i].keys);	free(tbl->cells[i].hashes);
		free(tbl->cells[i].data);	free(tbl->cells[i].next);
		tbl->cells[i].keys = NULL;	tbl->cells[i].hashes = NULL;
		tbl->cells[i].data = NULL;	tbl->cells[i].next = NULL;

		tbl->cells[i].tail = tbl->cells[i].free = UNDEF;
		tbl->cells[i].size = tbl->cells[i].capacity = 0;
	}

	free(tbl->cells);
}
