CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=c11

TARGETS = test_kv

all: $(TARGETS)

test_kv: kv_store.c test_kv.c kv_store.h
	$(CC) $(CFLAGS) -o $@ kv_store.c test_kv.c

clean:
	rm -f $(TARGETS) kv_test.txt

.PHONY: all clean
