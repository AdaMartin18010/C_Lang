CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=c11

TARGETS = test_lockfree

all: $(TARGETS)

test_lockfree: spsc_ring.c test_lockfree.c spsc_ring.h
	$(CC) $(CFLAGS) -o $@ spsc_ring.c test_lockfree.c

clean:
	rm -f $(TARGETS)

.PHONY: all clean
