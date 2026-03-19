/*
 * Auto-generated from: 12_Practice_Exercises\13_Real_World_Projects.md
 * Line: 3414
 * Language: c
 * Block ID: 21fd25c1
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#ifndef BTREE_H
#define BTREE_H

#include <stdint.h>
#include <stdbool.h>
#include "db.h"

#define BTREE_MIN_DEGREE 4  /* Minimum degree (t) */
#define BTREE_MAX_KEYS (2 * BTREE_MIN_DEGREE - 1)
#define BTREE_MAX_CHILDREN (2 * BTREE_MIN_DEGREE)

/* B+ tree node types */
typedef enum {
    NODE_INTERNAL = 1,
    NODE_LEAF = 2
} node_type_t;

/* B+ tree node structure (stored in page) */
typedef struct btree_node {
    node_type_t type;
    int num_keys;
    uint32_t keys[BTREE_MAX_KEYS];  /* Page numbers or row IDs */
    uint32_t children[BTREE_MAX_CHILDREN];  /* Child page numbers */
    uint32_t next;  /* Next leaf (for range scans) */
    uint32_t parent;  /* Parent page number */
    bool is_root;
} btree_node_t;

/* B+ tree handle */
typedef struct btree {
    db_t *db;
    uint32_t root_page;
    int (*compare)(const void *a, const void *b);
} btree_t;

/* B+ tree operations */
btree_t* btree_create(db_t *db, int (*compare)(const void*, const void*));
void btree_destroy(btree_t *tree);
int btree_insert(btree_t *tree, const void *key, uint32_t value);
int btree_delete(btree_t *tree, const void *key);
uint32_t btree_search(btree_t *tree, const void *key);
int btree_range_search(btree_t *tree, const void *start, const void *end,
                       uint32_t *results, int max_results);

/* Node operations */
btree_node_t* btree_load_node(btree_t *tree, uint32_t page_num);
int btree_save_node(btree_t *tree, btree_node_t *node, uint32_t page_num);
uint32_t btree_allocate_node(btree_t *tree);
void btree_free_node(btree_t *tree, uint32_t page_num);

/* Tree traversal */
typedef struct btree_cursor {
    btree_t *tree;
    uint32_t current_page;
    int current_idx;
} btree_cursor_t;

btree_cursor_t* btree_cursor_create(btree_t *tree);
void btree_cursor_destroy(btree_cursor_t *cursor);
int btree_cursor_first(btree_cursor_t *cursor);
int btree_cursor_last(btree_cursor_t *cursor);
int btree_cursor_next(btree_cursor_t *cursor);
int btree_cursor_prev(btree_cursor_t *cursor);
int btree_cursor_seek(btree_cursor_t *cursor, const void *key);

#endif /* BTREE_H */
