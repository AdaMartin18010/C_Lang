CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=c11

TARGETS = test_arena

all: $(TARGETS)

test_arena: arena.c test_arena.c arena.h
	$(CC) $(CFLAGS) -o $@ arena.c test_arena.c

clean:
	rm -f $(TARGETS)

.PHONY: all clean
