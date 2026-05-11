CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=c11

TARGETS = test_parser http_server

all: $(TARGETS)

test_parser: http_parser.c test_parser.c http_parser.h
	$(CC) $(CFLAGS) -o $@ http_parser.c test_parser.c

http_server: http_parser.c http_server.c http_parser.h
	$(CC) $(CFLAGS) -o $@ http_parser.c http_server.c

clean:
	rm -f $(TARGETS)

.PHONY: all clean
