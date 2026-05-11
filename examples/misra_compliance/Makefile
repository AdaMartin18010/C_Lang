CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=c11 -Wconversion

TARGETS = type_safety memory_safety control_flow function_safety

all: $(TARGETS)

type_safety: type_safety.c
	$(CC) $(CFLAGS) -o $@ $<

memory_safety: memory_safety.c
	$(CC) $(CFLAGS) -o $@ $<

control_flow: control_flow.c
	$(CC) $(CFLAGS) -o $@ $<

function_safety: function_safety.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(TARGETS)

.PHONY: all clean
