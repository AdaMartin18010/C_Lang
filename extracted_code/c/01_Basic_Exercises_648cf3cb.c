/*
 * Auto-generated from: 12_Practice_Exercises\01_Basic_Exercises.md
 * Line: 1321
 * Language: c
 * Block ID: 648cf3cb
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

typedef struct Node {
    int data;
    struct Node *next;
} Node;

Node* create_node(int data);
void insert_head(Node **head, int data);
void insert_tail(Node **head, int data);
void delete_node(Node **head, int data);
Node* find(Node *head, int data);
void print_list(Node *head);
void free_list(Node **head);
