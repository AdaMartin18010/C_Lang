CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=c11

TARGETS = tcp_server tcp_client

all: $(TARGETS)

tcp_server: tcp_server.c
	$(CC) $(CFLAGS) -o $@ $<

tcp_client: tcp_client.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(TARGETS)

.PHONY: all clean
