CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=c11 -D_FORTIFY_SOURCE=2

TARGETS = cve_cases

all: $(TARGETS)

cve_cases: cve_cases.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(TARGETS)

.PHONY: all clean
