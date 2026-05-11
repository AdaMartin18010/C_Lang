CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=c11

TARGETS = opaque_pointer api_versioning plugin_system

all: $(TARGETS)

opaque_pointer: opaque_pointer.c
	$(CC) $(CFLAGS) -o $@ $<

api_versioning: api_versioning.c
	$(CC) $(CFLAGS) -o $@ $<

plugin_system: plugin_system.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(TARGETS)

.PHONY: all clean
