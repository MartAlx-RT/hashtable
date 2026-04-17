#include "table.h"
#include <stdint.h>
#include <stdio.h>

int main(void)
{
	FILE *f = NULL;
	tbl_t tbl = {.size = 10};
	tbl_init(&tbl, 10);
	tbl_add("hello", 52, &tbl);	//TODO (char *) -> (key_t)
	tbl_add("gavno", 34, &tbl);
	tbl_add("parasha", 25, &tbl);
	tbl_add("world", 43, &tbl);

	printf("data = %lu\n", tbl_find((key_t){"hello"}, &tbl));
	//tbl_find((key_t){"hello!"}, &tbl);

	tbl_deinit(&tbl);
	return 0;
}
