CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=c11

TARGETS = vtable_polymorphism state_machine observer

all: $(TARGETS)

vtable_polymorphism: vtable_polymorphism.c
	$(CC) $(CFLAGS) -o $@ $<

state_machine: state_machine.c
	$(CC) $(CFLAGS) -o $@ $<

observer: observer.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(TARGETS)

.PHONY: all clean
