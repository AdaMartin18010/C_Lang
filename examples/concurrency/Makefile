CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=c11

TARGETS = c11_threads producer_consumer atomic_operations

all: $(TARGETS)

c11_threads: c11_threads.c
	$(CC) $(CFLAGS) -o $@ $<

producer_consumer: producer_consumer.c
	$(CC) $(CFLAGS) -o $@ $<

atomic_operations: atomic_operations.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(TARGETS)

.PHONY: all clean
