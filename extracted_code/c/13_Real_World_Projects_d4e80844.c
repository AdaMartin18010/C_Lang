/*
 * Auto-generated from: 12_Practice_Exercises\13_Real_World_Projects.md
 * Line: 3573
 * Language: c
 * Block ID: d4e80844
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * B+ Tree Implementation
 *
 * A B+ tree with the following properties:
 * - All data stored in leaf nodes
 * - Internal nodes only store keys for navigation
 * - Leaf nodes linked for efficient range scans
 * - Minimum degree t = 4
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "btree.h"
#include "pager.h"

#define MIN_KEYS (BTREE_MIN_DEGREE - 1)
#define MAX_KEYS (2 * BTREE_MIN_DEGREE - 1)

static btree_node_t* load_node(btree_t *tree, uint32_t page_num) {
    pager_t *pager = (pager_t*)tree->db;  /* Simplified */
    page_t *page = pager_get_page(pager, page_num);
    if (!page) return NULL;

    btree_node_t *node = malloc(sizeof(btree_node_t));
    if (!node) {
        pager_release_page(pager, page);
        return NULL;
    }

    memcpy(node, page->data, sizeof(btree_node_t));
    pager_release_page(pager, page);
    return node;
}

static int save_node(btree_t *tree, btree_node_t *node, uint32_t page_num) {
    pager_t *pager = (pager_t*)tree->db;
    page_t *page = pager_get_page(pager, page_num);
    if (!page) return -1;

    memcpy(page->data, node, sizeof(btree_node_t));
    pager_mark_dirty(pager, page);
    pager_release_page(pager, page);
    return 0;
}

btree_t* btree_create(db_t *db, int (*compare)(const void*, const void*)) {
    btree_t *tree = calloc(1, sizeof(btree_t));
    if (!tree) return NULL;

    tree->db = db;
    tree->compare = compare;

    /* Allocate root node */
    pager_t *pager = (pager_t*)db;
    page_t *root_page = pager_allocate_page(pager);
    if (!root_page) {
        free(tree);
        return NULL;
    }

    btree_node_t root = {0};
    root.type = NODE_LEAF;
    root.is_root = true;
    root.num_keys = 0;
    root.next = 0;
    root.parent = 0;

    memcpy(root_page->data, &root, sizeof(root));
    pager_mark_dirty(pager, root_page);

    tree->root_page = root_page->number;
    pager_release_page(pager, root_page);

    return tree;
}

void btree_destroy(btree_t *tree) {
    free(tree);
}

static int find_key_index(btree_node_t *node, btree_t *tree,
                          const void *key, uint32_t *page_num) {
    int i = 0;
    while (i < node->num_keys) {
        void *node_key = &node->keys[i];  /* Simplified */
        int cmp = tree->compare(key, node_key);

        if (cmp == 0) {
            *page_num = node->keys[i];
            return i;
        }
        if (cmp < 0) break;
        i++;
    }
    *page_num = 0;
    return -1;  /* Not found, would be at position i */
}

uint32_t btree_search(btree_t *tree, const void *key) {
    uint32_t page_num = tree->root_page;

    while (page_num != 0) {
        btree_node_t *node = load_node(tree, page_num);
        if (!node) return 0;

        uint32_t result;
        int idx = find_key_index(node, tree, key, &result);

        if (idx >= 0) {
            free(node);
            return result;  /* Found in leaf */
        }

        if (node->type == NODE_LEAF) {
            free(node);
            return 0;  /* Not found */
        }

        /* Navigate to child */
        uint32_t next_page = node->children[idx + 1];
        free(node);
        page_num = next_page;
    }

    return 0;
}

static void split_child(btree_t *tree, btree_node_t *parent, int idx,
                        uint32_t child_page) {
    btree_node_t *child = load_node(tree, child_page);
    if (!child) return;

    pager_t *pager = (pager_t*)tree->db;
    page_t *new_page = pager_allocate_page(pager);
    if (!new_page) {
        free(child);
        return;
    }

    btree_node_t new_node = {0};
    new_node.type = child->type;
    new_node.num_keys = MIN_KEYS;
    new_node.parent = parent->children[idx];
    new_node.is_root = false;

    /* Move second half to new node */
    for (int i = 0; i < MIN_KEYS; i++) {
        new_node.keys[i] = child->keys[i + BTREE_MIN_DEGREE];
    }

    if (child->type == NODE_INTERNAL) {
        for (int i = 0; i < BTREE_MIN_DEGREE; i++) {
            new_node.children[i] = child->children[i + BTREE_MIN_DEGREE];
        }
    } else {
        new_node.next = child->next;
        child->next = new_page->number;
    }

    child->num_keys = MIN_KEYS;

    /* Make room in parent */
    for (int i = parent->num_keys; i > idx; i--) {
        parent->children[i + 1] = parent->children[i];
        parent->keys[i] = parent->keys[i - 1];
    }

    parent->children[idx + 1] = new_page->number;
    parent->keys[idx] = child->keys[BTREE_MIN_DEGREE - 1];
    parent->num_keys++;

    /* Save all modified nodes */
    save_node(tree, child, child_page);
    save_node(tree, &new_node, new_page->number);
    save_node(tree, parent, parent->children[idx]);

    free(child);
    pager_release_page(pager, new_page);
}

static void insert_non_full(btree_t *tree, uint32_t page_num,
                            const void *key, uint32_t value) {
    btree_node_t *node = load_node(tree, page_num);
    if (!node) return;

    int i = node->num_keys - 1;

    if (node->type == NODE_LEAF) {
        /* Find insertion point */
        while (i >= 0 && tree->compare(key, &node->keys[i]) < 0) {
            node->keys[i + 1] = node->keys[i];
            i--;
        }

        node->keys[i + 1] = value;  /* Simplified */
        node->num_keys++;
        save_node(tree, node, page_num);
    } else {
        /* Find child to insert into */
        while (i >= 0 && tree->compare(key, &node->keys[i]) < 0) {
            i--;
        }
        i++;

        btree_node_t *child = load_node(tree, node->children[i]);
        if (!child) {
            free(node);
            return;
        }

        if (child->num_keys == MAX_KEYS) {
            split_child(tree, node, i, node->children[i]);

            if (tree->compare(key, &node->keys[i]) > 0) {
                i++;
            }
            free(child);
            child = load_node(tree, node->children[i]);
        }

        save_node(tree, node, page_num);
        free(node);

        insert_non_full(tree, node->children[i], key, value);
        free(child);
        return;
    }

    free(node);
}

int btree_insert(btree_t *tree, const void *key, uint32_t value) {
    btree_node_t *root = load_node(tree, tree->root_page);
    if (!root) return -1;

    if (root->num_keys == MAX_KEYS) {
        /* Root is full, create new root */
        pager_t *pager = (pager_t*)tree->db;
        page_t *new_root_page = pager_allocate_page(pager);
        if (!new_root_page) {
            free(root);
            return -1;
        }

        btree_node_t new_root = {0};
        new_root.type = NODE_INTERNAL;
        new_root.is_root = true;
        new_root.num_keys = 0;
        new_root.children[0] = tree->root_page;

        root->is_root = false;
        root->parent = new_root_page->number;

        save_node(tree, root, tree->root_page);

        uint32_t old_root = tree->root_page;
        tree->root_page = new_root_page->number;

        split_child(tree, &new_root, 0, old_root);
        save_node(tree, &new_root, tree->root_page);

        free(root);
        pager_release_page(pager, new_root_page);

        root = load_node(tree, tree->root_page);
    }

    insert_non_full(tree, tree->root_page, key, value);
    free(root);
    return 0;
}

/* Cursor implementation */
btree_cursor_t* btree_cursor_create(btree_t *tree) {
    btree_cursor_t *cursor = calloc(1, sizeof(btree_cursor_t));
    if (!cursor) return NULL;

    cursor->tree = tree;
    cursor->current_page = 0;
    cursor->current_idx = -1;

    return cursor;
}

void btree_cursor_destroy(btree_cursor_t *cursor) {
    free(cursor);
}

int btree_cursor_first(btree_cursor_t *cursor) {
    btree_node_t *node = load_node(cursor->tree, cursor->tree->root_page);
    if (!node) return -1;

    /* Go to leftmost leaf */
    while (node->type == NODE_INTERNAL && node->num_keys > 0) {
        uint32_t child_page = node->children[0];
        free(node);
        node = load_node(cursor->tree, child_page);
        if (!node) return -1;
    }

    if (node->type == NODE_LEAF && node->num_keys > 0) {
        cursor->current_page = node->keys[0];  /* Simplified */
        cursor->current_idx = 0;
        free(node);
        return 0;
    }

    free(node);
    return -1;
}

int btree_cursor_next(btree_cursor_t *cursor) {
    if (cursor->current_page == 0) return -1;

    btree_node_t *node = load_node(cursor->tree, cursor->current_page);
    if (!node) return -1;

    cursor->current_idx++;

    if (cursor->current_idx >= node->num_keys) {
        /* Move to next leaf */
        uint32_t next_page = node->next;
        free(node);

        if (next_page == 0) {
            cursor->current_page = 0;
            cursor->current_idx = -1;
            return -1;
        }

        cursor->current_page = next_page;
        cursor->current_idx = 0;
        return 0;
    }

    free(node);
    return 0;
}

int btree_cursor_seek(btree_cursor_t *cursor, const void *key) {
    uint32_t page_num = btree_search(cursor->tree, key);
    if (page_num == 0) return -1;

    cursor->current_page = page_num;
    cursor->current_idx = 0;
    return 0;
}
