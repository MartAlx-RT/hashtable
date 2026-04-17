#include "table.h"
#include <stddef.h>


#define write_head(l)				fprintf(f, "digraph cell%lu\n{rankdir=LR;\n\n\n", l)
#define write_tail()				fprintf(f, "}\n")
#define write_node(l, key, data, next, clr)	fprintf(f, "label%lu[shape=record, style=\"rounded, filled\", fillcolor="clr", label=\"i = %lu | {key=\\{%s\\} | data=%lu | n=%lu}\"];\n", l, l, key, data, next)
#define write_true_node(l, key, data, next)	write_node(l, key, data, next, "green")
#define write_wrong_node(l, key, data, next)	write_node(l, key, data, next, "red")
#define write_link(l1, l2)			fprintf(f, "label%lu->label%lu;\n", l1, l2)
void cell_dump(tbl_cell_t *cell, FILE *f)
{
	static size_t n_calls = 0;
	assert(cell);	assert(f);

	fprintf(stderr, "free = %lu\tsize = %lu\tcapacity = %lu\n\n", cell->free, cell->size, cell->capacity);

	write_head(n_calls);
	for(size_t i=0; i<=cell->capacity; i++)
	{
		write_true_node(i, cell->keys[i], cell->data[i], cell->next[i]);
		write_link(i, cell->next[i]);
	}
	write_tail();
	
	n_calls++;
}

void tbl_dump(tbl_t *tbl, FILE *f)
{
	assert(tbl);	assert(f);

	for(size_t i=0; i<tbl->size; i++)
		cell_dump(&(tbl->cells[i]), f);
}
#undef write_tail
#undef write_head
#undef write_node
#undef write_true_node
#undef write_wrong_node
#undef write_link
