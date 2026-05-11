CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=c11

TARGETS = mmap_example signal_handler process_fork epoll_server

all: $(TARGETS)

mmap_example: mmap_example.c
	$(CC) $(CFLAGS) -o $@ $<

signal_handler: signal_handler.c
	$(CC) $(CFLAGS) -o $@ $<

process_fork: process_fork.c
	$(CC) $(CFLAGS) -o $@ $<

epoll_server: epoll_server.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(TARGETS)

.PHONY: all clean
