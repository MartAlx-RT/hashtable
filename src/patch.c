#include <sys/mman.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <assert.h>
#include <string.h>

int main(int argc, char *argv[])
{
	if(argc != 2)	{ return fprintf(stderr, "usage:\t%s <file>\n", argv[0]); }

	int fd = open(argv[1], O_RDWR);
	if(fd < 0)	{ return fprintf(stderr, "file doesn't exist\n"); }
	struct stat finfo = {};	fstat(fd, &finfo);

	char *f = mmap(NULL, finfo.st_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	assert(f);

	char *s = f;			// substituting character
	while((s = strchr(s, ' ')))	*s++ = '\0';

	return munmap(f, finfo.st_size);
}
