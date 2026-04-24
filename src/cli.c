#include "table.h"
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <ctype.h>

typedef enum cli_mode_t
{
	MODE_QUIT = 'q',
	MODE_ADD = 'a',
	MODE_DEL = 'd',
	MODE_FIND = 'f',
	MODE_HELP = '\n',
} cli_mode_t;

const size_t TBL_SIZE = 3999;
const size_t CELL_INIT_SIZE = 100;

#define ERR(func)	do { perror(func); return 1; } while(0)
void cli(tbl_t *tbl, tbl_hash_t (*tbl_hash)(const tbl_key_t));
void load_db(const char *f, const char *eof, tbl_t *tbl, tbl_hash_t (*tbl_hash)(const tbl_key_t));
const char *strcpy_64(tbl_key_t dst, const char *src, const char *eof);
tbl_hash_t (*const tbl_hash)(const tbl_key_t) = tbl_crc32intrin_hash;

int main(void)
{
	// opening
	int fd = open("search.txt", O_RDONLY);
	if(fd < 0)	ERR("open");
	struct stat finfo = {};	fstat(fd, &finfo);

	const char *f = (const char *)mmap(NULL, finfo.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	if(f == NULL)	ERR("mmap");

	tbl_t tbl = {};
	tbl_init(TBL_SIZE, CELL_INIT_SIZE, &tbl);

	// loading database
	load_db(f, f+finfo.st_size, &tbl, tbl_hash);
	printf("load factor: %lg\n", tbl_get_ldfactor(&tbl));
	cli(&tbl, tbl_hash);

	return 0;
}

void cli(tbl_t *tbl, tbl_hash_t (*tbl_hash)(const tbl_key_t))
{
	assert(tbl);	assert(tbl_hash);

	cli_mode_t mode = MODE_QUIT;
	tbl_key_t key = "";
	tbl_data_t data = 0;
	while(1)
	{
		memset(key, '\0', sizeof(tbl_key_t));
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
				tbl_add(key, tbl, tbl_hash);
				break;
			case MODE_DEL:
				printf("(key)>");
				scanf("%63s", key);

				tbl_del(key, tbl, tbl_hash);
				break;
			case MODE_FIND:
				printf("(key)>");
				scanf("%63s", key);

				data = tbl_find(key, tbl, tbl_hash);
				if(!errno)	printf("`%s`: %lu\n", key, data);
				break;
			default:
				printf("invalid mode\n");
				break;
		}

		if(errno)	{ perror("search-cli"); errno = 0; }
		putchar('\n');
		while(getchar() != '\n');
	}
}

void load_db(const char *f, const char *eof, tbl_t *tbl, tbl_hash_t (*tbl_hash)(const tbl_key_t))
{
	assert(f);	assert(tbl);	assert(tbl_hash);

	tbl_key_t key = "";

	while((f = strcpy_64(key, f, eof)))
		tbl_add(key, tbl, tbl_hash);
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

