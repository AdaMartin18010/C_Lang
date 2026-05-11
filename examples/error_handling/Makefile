CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=c11

TARGETS = error_code_system goto_cleanup raii_simulation setjmp_example

all: $(TARGETS)

error_code_system: error_code_system.c
	$(CC) $(CFLAGS) -o $@ $<

goto_cleanup: goto_cleanup.c
	$(CC) $(CFLAGS) -o $@ $<

raii_simulation: raii_simulation.c
	$(CC) $(CFLAGS) -o $@ $<

setjmp_example: setjmp_example.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(TARGETS)

.PHONY: all clean
