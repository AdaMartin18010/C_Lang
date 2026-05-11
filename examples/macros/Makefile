CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=c11

TARGETS = x_macros generic_selection stringify_concat

all: $(TARGETS)

x_macros: x_macros.c
	$(CC) $(CFLAGS) -o $@ $<

generic_selection: generic_selection.c
	$(CC) $(CFLAGS) -o $@ $<

stringify_concat: stringify_concat.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(TARGETS)

.PHONY: all clean
