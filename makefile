.PHONY : all clean release debug
INC = -Iinclude
VPATH = src

CC ?= gcc

HASH_FUNC ?= const_hash.c
OUTPUT ?= a.out

FILES = test.c table.c
FILES += $(HASH_FUNC)

CFLAGS = -Wall -Wextra -Wpedantic -g -march=native
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
