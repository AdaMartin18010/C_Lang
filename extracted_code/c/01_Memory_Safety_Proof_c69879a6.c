/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\11_CompCert_Verification\05_Practical_Verification_Cases\01_Memory_Safety_Proof.md
 * Line: 117
 * Language: c
 * Block ID: c69879a6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

typedef struct Node {
    int data;
    struct Node* next;
} Node;

Node* list_prepend(Node* head, int value);
void list_free(Node* head);
