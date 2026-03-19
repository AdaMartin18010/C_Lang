/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\11_CompCert_Verification\06_Coq_Examples\01_Coq_Verification_Cases.md
 * Line: 3563
 * Language: c
 * Block ID: b77ad5a6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* File: c_impl/separation_logic_examples.c */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/* ============================================================ */
/* 1. 单链表节点 */
/* ============================================================ */

typedef struct Node {
    int value;
    struct Node* next;
} Node;

/* 
 * 规范: {list(p, xs)}
 *       list_length(p)
 *       {ret = len(xs) /\\ list(p, xs)}
 */
int list_length_iter(Node* p) {
    int len = 0;
    while (p != NULL) {
        len++;
        p = p->next;
    }
    return len;
}

/*
 * 规范: {list(p, xs) * x |-> _}
 *       list_prepend(p, x)
 *       {list(ret, x::xs)}
 */
Node* list_prepend(Node* p, int value) {
    Node* new_node = (Node*)malloc(sizeof(Node));
    new_node->value = value;
    new_node->next = p;
    return new_node;
}

/*
 * 规范: {list(p, x::xs)}
 *       list_pop(p)
 *       {ret = x /\\ list(p', xs) /\\ p |-> _}
 */
int list_pop(Node** p) {
    assert(*p != NULL);
    Node* head = *p;
    int value = head->value;
    *p = head->next;
    free(head);
    return value;
}

/*
 * 规范: {list(p, xs)}
 *       list_append(p, x)
 *       {list(ret, xs ++ [x])}
 */
Node* list_append(Node* p, int value) {
    Node* new_node = (Node*)malloc(sizeof(Node));
    new_node->value = value;
    new_node->next = NULL;
    
    if (p == NULL) {
        return new_node;
    }
    
    Node* current = p;
    while (current->next != NULL) {
        current = current->next;
    }
    current->next = new_node;
    return p;
}

/*
 * 规范: {list(p, xs)}
 *       free_list(p)
 *       {emp}
 */
void free_list(Node* p) {
    while (p != NULL) {
        Node* temp = p;
        p = p->next;
        free(temp);
    }
}

/* ============================================================ */
/* 2. 树形结构 */
/* ============================================================ */

typedef struct TreeNode {
    int value;
    struct TreeNode* left;
    struct TreeNode* right;
} TreeNode;

/*
 * 规范: {tree(t)}
 *       tree_size(t)
 *       {ret = size(tree)}
 */
int tree_size(TreeNode* t) {
    if (t == NULL) return 0;
    return 1 + tree_size(t->left) + tree_size(t->right);
}

/* ============================================================ */
/* 3. 内存分配器（带分离逻辑规范） */
/* ============================================================ */

typedef struct Block {
    size_t size;
    struct Block* next;
    char data[];  /* 柔性数组 */
} Block;

static Block* free_list = NULL;

/*
 * 规范: {emp}
 *       allocator_init(size)
 *       {block(0, size) * free_list |-> 0}
 */
void allocator_init(size_t size) {
    free_list = (Block*)malloc(sizeof(Block) + size);
    free_list->size = size;
    free_list->next = NULL;
}

/*
 * 规范: {free_list |-> fl * list(fl, blocks)}
 *       allocator_alloc(n)
 *       {ret = null /\\ free_list |-> fl * list(fl, blocks)}
 *       \/
 *       {ret = addr /\\ addr |-> n /\\ free_list |-> fl' * list(fl', blocks')}
 */
void* allocator_alloc(size_t n) {
    Block** current = &free_list;
    
    while (*current != NULL) {
        if ((*current)->size >= n) {
            Block* block = *current;
            if (block->size == n) {
                *current = block->next;
            } else {
                /* 分割块 */
                Block* new_block = (Block*)((char*)block + sizeof(Block) + n);
                new_block->size = block->size - n - sizeof(Block);
                new_block->next = block->next;
                *current = new_block;
                block->size = n;
            }
            return block->data;
        }
        current = &(*current)->next;
    }
    return NULL;
}

/*
 * 规范: {addr |-> n /\\ free_list |-> fl * list(fl, blocks)}
 *       allocator_free(addr, n)
 *       {free_list |-> fl' * list(fl', blocks')}
 *       其中blocks'是blocks加上新的空闲块
 */
void allocator_free(void* ptr, size_t n) {
    Block* block = (Block*)((char*)ptr - sizeof(Block));
    block->size = n;
    block->next = free_list;
    free_list = block;
}

/* ============================================================ */
/* 4. 测试函数 */
/* ============================================================ */

void test_list_operations() {
    Node* list = NULL;
    
    /* 测试prepend */
    list = list_prepend(list, 3);
    list = list_prepend(list, 2);
    list = list_prepend(list, 1);
    
    assert(list_length_iter(list) == 3);
    
    /* 测试pop */
    assert(list_pop(&list) == 1);
    assert(list_pop(&list) == 2);
    
    /* 测试append */
    list = list_append(list, 4);
    assert(list_length_iter(list) == 2);
    
    free_list(list);
    printf("List operations tests passed!\n");
}
