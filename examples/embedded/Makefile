CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=c11

TARGETS = memory_mapped_io bitfields_register interrupt_simulation

all: $(TARGETS)

memory_mapped_io: memory_mapped_io.c
	$(CC) $(CFLAGS) -o $@ $<

bitfields_register: bitfields_register.c
	$(CC) $(CFLAGS) -o $@ $<

interrupt_simulation: interrupt_simulation.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(TARGETS)

.PHONY: all clean
