.PHONY : all clean release debug
INC = -Iinclude

CC ?= gcc

HASH_FUNC ?= src/const_hash.c
TEST_FILE ?= test/test.c
OUTPUT ?= a.out

FILES = src/table.c $(HASH_FUNC) $(TEST_FILE)

CFLAGS = -Wall -Wextra -Wpedantic -g -march=native -DNDEBUG
DEBUG_FLAGS = -fsanitize=address,leak -O0 -g
CFLAGS += $(INC)
DEBUG_FLAGS += $(INC)

all:	debug release

debug:		$(FILES)
	$(CC) $(DEBUG_FLAGS) $^ -o $(OUTPUT)

release:	$(FILES)
	$(CC) $(CFLAGS) -O3 $^ -o $(OUTPUT)

clean:
	rm -f debug release
