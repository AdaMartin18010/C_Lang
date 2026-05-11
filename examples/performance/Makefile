CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=c11

TARGETS = performance

all: $(TARGETS)

performance: performance.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(TARGETS)

.PHONY: all clean
