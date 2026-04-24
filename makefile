.PHONY : all clean release debug
INC = -Iinclude

CC ?= gcc

TEST_FILE ?= test/test.c
OUTPUT ?= a.out

FILES = src/table.c src/hash_funcs.c src/crc32asm_hash.s $(TEST_FILE)

CFLAGS = -Wall -Wextra -Wpedantic -g -march=native -DNDEBUG -lm
CFLAGS += $(INC)
DEBUG_FLAGS = $(CFLAGS) -fsanitize=address,leak -O0 -g -lm

all:	debug release

debug:		$(FILES)
	$(CC) $(DEBUG_FLAGS) $^ -o $(OUTPUT)

release:	$(FILES)
	$(CC) $(CFLAGS) -O3 $^ -o $(OUTPUT)

clean:
	rm -f debug release
