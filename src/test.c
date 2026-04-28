#include "table.h"
#include "hash_funcs.h"
#include <fcntl.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <ctype.h>
#include <sys/types.h>
#include <math.h>


#define HASH_FUNC_INIT(f, s, n)	{ .func = f, .name = #f, .TBL_SIZE = s, .N_CYCLES = n }
static const tbl_test_t TBL_TESTS[] =
{
	HASH_FUNC_INIT(tbl_const_hash, 4001, 2),
	HASH_FUNC_INIT(tbl_1a_hash, 4001, 2),
	HASH_FUNC_INIT(tbl_len_hash, 4001, 2),
	HASH_FUNC_INIT(tbl_sum_hash, 4001, 500),
	HASH_FUNC_INIT(tbl_sum_hash, 401, 500),
	HASH_FUNC_INIT(tbl_rol_hash, 4001, 500),
	HASH_FUNC_INIT(tbl_crc32_hash, 4001, 500),
	HASH_FUNC_INIT(tbl_crc32asm_hash, 4001, 500),
	HASH_FUNC_INIT(tbl_crc32intrin_hash, 4001, 500),
	HASH_FUNC_INIT(tbl_crc32asminline_hash, 4001, 500)
};

const size_t CELL_INIT_SIZE = 100;

#define ERR(func)	do { perror(func); return 1; } while(0)
#define SQ(x)		((x)*(x))
const char *strcpy_64(tbl_key_t dst, const char *src, const char *eof);			// compares 64-ltrs words
void search(const tbl_key_t srch_keys[], const size_t n_srch_keys, tbl_t *tbl);		// search test
size_t load_db(const char *f, const char *eof, tbl_t *tbl, tbl_key_t *added_keys[]);	// loads database
void srch_test(const char *f, const char *eof);

int main(void)
{
	// opening
	int fd = open("test.txt", O_RDONLY);
	if(fd < 0)	ERR("open");
	struct stat finfo = {};	fstat(fd, &finfo);

	const char *f = (const char *)mmap(NULL, finfo.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	if(f == NULL)	ERR("mmap");

	srch_test(f, f+finfo.st_size);

	if(munmap((void *)f, finfo.st_size))	ERR("munmap");
	return 0;
}

size_t load_db(const char *f, const char *eof, tbl_t *tbl, tbl_key_t *added_keys[])
{
	assert(f);	assert(eof);	assert(tbl);	assert(added_keys);

	tbl_key_t key = "";
	size_t n_keys = 0, added_keys_capacity = tbl->size;
	*added_keys = (tbl_key_t *)calloc(added_keys_capacity, sizeof(tbl_key_t));
	assert(*added_keys);

	while((f = strcpy_64(key, f, eof)))
	{
		tbl_add(key, tbl);

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

void srch_test(const char *f, const char *eof)
{
	tbl_t tbl = {};
	double sum_time = 0, sum_time2 = 0;	// sum(time_i), sum(time_i ^2)
	tbl_key_t *srch_keys = NULL;
	size_t n_srch_keys = 0;

	for(size_t test = 0; test < sizeof(TBL_TESTS)/sizeof(TBL_TESTS[0]); test++)
	{
		tbl_init(TBL_TESTS[test].TBL_SIZE, CELL_INIT_SIZE, TBL_TESTS[test].func, &tbl);
		n_srch_keys = load_db(f, eof, &tbl, &srch_keys);

#ifdef TBL_TEST_HASH
		printf(":%s\n", TBL_TESTS[test].name);
		for(size_t i = 0; i < tbl.size; i++)
			printf("%s,%lu\n", tbl.cells[i].size);
#else
		for(size_t cycle = 0; cycle < TBL_TESTS[test].N_CYCLES; cycle++)
		{
			uint64_t time = __rdtsc();
			for(size_t i = 0; i < n_srch_keys; i++)
			{
				if(tbl_find(srch_keys[i], &tbl) == UNDEF)
				{
					fprintf(stderr, "key[%lu] = {%s}, test failed!\n", i, srch_keys[i]);
					errno = 0;
					return;
				}
			}
			time = __rdtsc()-time;

			sum_time += (double)time;	sum_time2 += SQ((double)time);
		}

		fprintf(stderr, "%s,%lg,%lg\n",
				TBL_TESTS[test].name,
				1e-8*sum_time/TBL_TESTS[test].N_CYCLES,
				1e-8*sqrt(sum_time2/TBL_TESTS[test].N_CYCLES - SQ(sum_time/TBL_TESTS[test].N_CYCLES)));
#endif
	}
}
