/*
 * Auto-generated from: 12_Practice_Exercises\11_Zig_C_Migration_Projects.md
 * Line: 57
 * Language: c
 * Block ID: d596f5e2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// list.h
typedef struct Node {
    int data;
    struct Node *next;
} Node;

typedef struct {
    Node *head;
    Node *tail;
    size_t len;
} List;

List* list_create(void);
void list_destroy(List *list);
void list_append(List *list, int value);
bool list_remove(List *list, int value);
int* list_to_array(const List *list, size_t *len);
