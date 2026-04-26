#include "table.h"
#include <fcntl.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <ctype.h>
#include <math.h>

const size_t TBL_SIZE = 4001;
const size_t CELL_INIT_SIZE = 100;
const size_t N_TESTS = 500;

#define SQ(x)		((x)*(x))
#define ERR(func)	do { perror(func); return 1; } while(0)
const char *strcpy_64(tbl_key_t dst, const char *src, const char *eof);	// compare 64-ltrs words
size_t load_db(const char *f, const char *eof, tbl_t *tbl, tbl_key_t *added_keys[], tbl_hash_t (*tbl_hash)(const tbl_key_t));
void search_test(const tbl_key_t srch_keys[], const size_t n_srch_keys, tbl_t *tbl, tbl_hash_t (*tbl_hash)(const tbl_key_t));

static tbl_hash_t (*const TBL_HASH_FUNCS[])(const tbl_key_t) =
{ tbl_crc32intrin64_hash };

int main(void)
{
	// opening
	int fd = open("test.txt", O_RDONLY);
	if(fd < 0)	ERR("open");

	struct stat finfo = {};	fstat(fd, &finfo);
	const char *f = (const char *)mmap(NULL, finfo.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	if(f == NULL)	ERR("mmap");

	tbl_t tbl = {};

	for(size_t func = 0; func < sizeof(TBL_HASH_FUNCS)/sizeof(tbl_hash_t (*)(const tbl_key_t)); func++)
	{
		tbl_init(TBL_SIZE, CELL_INIT_SIZE, &tbl);

		// loading database
		tbl_key_t *added_keys = NULL;
		size_t n_added_keys = load_db(f, f+finfo.st_size, &tbl, &added_keys, TBL_HASH_FUNCS[func]);
//		fprintf(stderr, "ld factor = %lg\n", tbl_get_ldfactor(&tbl));

//		fprintf(stderr, "`%s`:\t", TBL_HASH_NAMES[func]);
		search_test(added_keys, n_added_keys, &tbl, TBL_HASH_FUNCS[func]);

		// deinit
		tbl_deinit(&tbl);
		free(added_keys);
	}

	if(munmap((void *)f, finfo.st_size))	ERR("munmap");
	return 0;
}

size_t load_db(const char *f, const char *eof, tbl_t *tbl, tbl_key_t *added_keys[], tbl_hash_t (*tbl_hash)(const tbl_key_t))
{
	assert(f);	assert(eof);	assert(tbl);	assert(added_keys);	assert(tbl_hash);

	tbl_key_t key = "";
	size_t n_keys = 0, added_keys_capacity = tbl->size;
	*added_keys = (tbl_key_t *)calloc(added_keys_capacity, sizeof(tbl_key_t));
	assert(*added_keys);

	while((f = strcpy_64(key, f, eof)))
	{
		tbl_add(key, tbl, tbl_hash);

		memcpy((*added_keys)[n_keys], key, sizeof(tbl_key_t));

		if(++n_keys >= added_keys_capacity)
		{
			added_keys_capacity = n_keys*2;
			*added_keys = (tbl_key_t *)reallocarray(*added_keys, added_keys_capacity, sizeof(tbl_key_t));
			assert(*added_keys);
		}
	}

	return n_keys;
}

const char *strcpy_64(tbl_key_t dst, const char *src, const char *eof)
{
	assert(dst);	assert(src);	assert(eof);

	memset(dst, '\0', sizeof(tbl_key_t));

	while(src < eof && isspace(*src))
		src++;
	if(src >= eof)	return NULL;

	const char *const src_beg = src;
	while(src-src_beg < MAX_S_LEN-1 && !isspace(*src) && src < eof)
		*dst++ = *src++;
	*dst = '\0';

	if(src >= eof)	return NULL;
	return src+1;
}

void search_test(const tbl_key_t srch_keys[], const size_t n_srch_keys, tbl_t *tbl, tbl_hash_t (*tbl_hash)(const tbl_key_t))
{
	assert(srch_keys);	assert(tbl);	assert(tbl_hash);

	double sum_time = 0, sum_time2 = 0;	// sum(time_i), sum(time_i ^2)

	for(size_t test = 0; test < N_TESTS; test++)
	{
		uint64_t time = __rdtsc();
		for(size_t i = 0; i < n_srch_keys; i++)
		{
			if(tbl_find(srch_keys[i], tbl, tbl_hash) == UNDEF)
			{
				fprintf(stderr, "key[%lu] = {%s}, test failed!\n", i, srch_keys[i]);
				errno = 0;
				return;
			}
		}
		time = __rdtsc()-time;

		sum_time += (double)time;	sum_time2 += SQ((double)time);
	}

//	fprintf(stderr, "function spent:\t(%lg +- %lg) clocks\n",
//			sum_time/N_TESTS,
//			sqrt(sum_time2/N_TESTS - SQ(sum_time/N_TESTS)));
	fprintf(stderr, "%lg,%lg\n",
			1e-8*sum_time/N_TESTS,
			1e-8*sqrt(sum_time2/N_TESTS - SQ(sum_time/N_TESTS)));
}
