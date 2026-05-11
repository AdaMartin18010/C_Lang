CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=c11

TARGETS = intrusive_list list_algorithms hash_table dynamic_array binary_tree

all: $(TARGETS)

intrusive_list: intrusive_list.c
	$(CC) $(CFLAGS) -o $@ $<

list_algorithms: list_algorithms.c
	$(CC) $(CFLAGS) -o $@ $<

hash_table: hash_table.c
	$(CC) $(CFLAGS) -o $@ $<

dynamic_array: dynamic_array.c
	$(CC) $(CFLAGS) -o $@ $<

binary_tree: binary_tree.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(TARGETS)

.PHONY: all clean
