CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=c11

TARGETS = bitmap_allocator bit_flags endianness

all: $(TARGETS)

bitmap_allocator: bitmap_allocator.c
	$(CC) $(CFLAGS) -o $@ $<

bit_flags: bit_flags.c
	$(CC) $(CFLAGS) -o $@ $<

endianness: endianness.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(TARGETS)

.PHONY: all clean
