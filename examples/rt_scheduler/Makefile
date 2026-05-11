CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=c11
LDFLAGS = -lm

TARGETS = test_scheduler

all: $(TARGETS)

test_scheduler: rt_scheduler.c test_scheduler.c rt_scheduler.h
	$(CC) $(CFLAGS) -o $@ rt_scheduler.c test_scheduler.c $(LDFLAGS)

clean:
	rm -f $(TARGETS)

.PHONY: all clean
