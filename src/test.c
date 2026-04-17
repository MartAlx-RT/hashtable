#include "table.h"
#include <asm-generic/errno.h>
#include <assert.h>
#include <fcntl.h>
#include <malloc.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <ctype.h>
#include <sys/types.h>
#include <valgrind/callgrind.h>
#include <x86intrin.h>

typedef enum test_mode_t
{
	MODE_QUIT = 'q',
	MODE_ADD = 'a',
	MODE_DEL = 'd',
	MODE_FIND = 'f',
	MODE_HELP = '\n',
} test_mode_t;

const size_t TBL_SIZE = 499;
const size_t CELL_INIT_SIZE = 100;

#define ERR(func)	do { perror(func); return 1; } while(0)
const char *strcpy_64(tbl_key_t dst, const char *src, const char *eof);	// compare 64-ltrs words
size_t load_db(const char *f, const char *eof, tbl_t *tbl, tbl_key_t *added_keys[]);
void test(tbl_key_t keys[], size_t n_keys, tbl_t *tbl);	// auto-test (searching all words in file)
void tui(tbl_t *tbl);					// tui for manually testing

int main(int argc, char *argv[])
{
	int fd = open("biblia.txt", O_RDONLY);
	if(fd < 0)	ERR("open");

	struct stat finfo = {};	fstat(fd, &finfo);
	const char *f = (const char *)mmap(NULL, finfo.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	if(f == NULL)	ERR("mmap");

	tbl_t tbl = { .size=TBL_SIZE };
	tbl_init(&tbl, CELL_INIT_SIZE);

	tbl_key_t *added_keys = NULL;
	size_t n_added_keys = load_db(f, f+finfo.st_size, &tbl, &added_keys);

	if(argc == 2 && !strcmp(argv[1], "--tui"))	tui(&tbl);
	else
	{
		uint64_t time = __rdtsc();
		test(added_keys, n_added_keys, &tbl);
		time = __rdtsc()-time;
		printf("testing finished: %lu clocks\n", time);
	}

	tbl_deinit(&tbl);
	if(munmap((void *)f, finfo.st_size))	ERR("munmap");
	return 0;
}

size_t load_db(const char *f, const char *eof, tbl_t *tbl, tbl_key_t *added_keys[])
{
	assert(f);	assert(tbl);	assert(added_keys);

	tbl_key_t key = "";
	size_t n_keys = 0, added_keys_capacity = tbl->size;
	*added_keys = (tbl_key_t *)calloc(added_keys_capacity, sizeof(tbl_key_t));
	assert(*added_keys);

	while(f)
	{
		f = strcpy_64(key, f, eof);
		tbl_add(key, tbl);

		strcpy((*added_keys)[n_keys], key);

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

	const char *const src_beg = src;

	while(src-src_beg < MAX_S_LEN-1 && !isspace(*src) && src < eof)
		*dst++ = *src++;
	*dst = '\0';

	if(src >= eof)	return NULL;
	return src+1;
}

void test(tbl_key_t keys[], size_t n_keys, tbl_t *tbl)
{
	assert(keys);	assert(tbl);

	for(volatile size_t i=0; i < n_keys; i++)
	{
		if(tbl_find(keys[i], tbl) == UNDEF)
		{
			errno = ENOKEY;
			return;
		}
	}
}

void tui(tbl_t *tbl)
{
	assert(tbl);

	test_mode_t mode = MODE_QUIT;
	tbl_key_t key = "";
	tbl_data_t data = 0;
	while(1)
	{
		printf("(mode)>");
		mode = getchar();

		switch(mode)
		{
			case MODE_HELP:
				printf("usage:\nq\tquit\na\tadd\nd\tdel\nf\tfind\n<CR>\thelp\n");
				break;
			case MODE_QUIT:
				return;
			case MODE_ADD:
				printf("(key)>");
				scanf("%63s", key);
				tbl_add(key, tbl);
				break;
			case MODE_DEL:
				printf("(key)>");
				scanf("%63s", key);

				tbl_del(key, tbl);
				break;
			case MODE_FIND:
				printf("(key)>");
				scanf("%63s", key);

				data = tbl_find(key, tbl);
				if(!errno)	printf("`%s`: %lu\n", key, data);
				break;
			default:
				printf("invalid mode\n");
				break;
		}

		if(errno)	{ perror("test-tui"); errno = 0; }
		putchar('\n');
		while(getchar() != '\n');
	}
}
