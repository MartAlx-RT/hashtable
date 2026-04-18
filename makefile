.PHONY : all clean release debug
INC = -Iinclude
VPATH = src

CC ?= gcc

HASH_FUNC ?= const_hash.c
FILES = test.c table.c

CFLAGS = -Wall -Wextra -Wpedantic -g -march=native
DEBUG_FLAGS = -fsanitize=address,leak -O0 -g
CFLAGS += $(INC)
DEBUG_FLAGS += $(INC)

all:	debug release

debug:		$(FILES) $(HASH_FUNC)
	$(CC) $(DEBUG_FLAGS) $^ -o $@

release:	$(FILES) $(HASH_FUNC)
	$(CC) $(CFLAGS) -O3 $^ -o $@

clean:
	rm -f debug release
