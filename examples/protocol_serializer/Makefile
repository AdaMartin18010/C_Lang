CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=c11

TARGETS = test_serializer

all: $(TARGETS)

test_serializer: serializer.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(TARGETS)

.PHONY: all clean
