.PHONY : all debug test cli
INC = -Iinclude

CC ?= gcc

FILES = src/table.c src/hash_funcs.c src/crc32asm_hash.s src/key_cmp.s

CFLAGS = -Wall -Wextra -Wpedantic -g -march=native -DNDEBUG -lm
CFLAGS += $(INC)
DEBUG_FLAGS = $(CFLAGS) -fsanitize=address,leak -O0 -g -lm

TEST_FILE = src/test.c

all:	debug release

debug:		$(FILES) $(TEST_FILE)
	$(CC) $(DEBUG_FLAGS) $^ -o tests/test

speed:		$(FILES) $(TEST_FILE)
	$(CC) $(CFLAGS) -O3 $^ -o tests/test

hash:		$(FILES) $(TEST_FILE)
	$(CC) $(CFLAGS) -O3 -DTBL_TEST_HASH $^ -o tests/test

cli:		$(FILES) src/cli.c
	$(CC) $(CFLAGS) -O3 $^ -o tests/test

clean:
	rm -f tests/test
