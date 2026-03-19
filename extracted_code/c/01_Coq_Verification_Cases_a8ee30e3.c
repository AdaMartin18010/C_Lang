/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\11_CompCert_Verification\06_Coq_Examples\01_Coq_Verification_Cases.md
 * Line: 2327
 * Language: c
 * Block ID: a8ee30e3
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* File: c_impl/memory_allocator.c */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

/* ============================================================ */
/* 1. 内存块定义 */
/* ============================================================ */

typedef struct Block {
    size_t start;
    size_t size;
    struct Block* next;
} Block;

typedef struct {
    Block* free_list;
    Block* used_list;
    size_t total_size;
} Allocator;

/* ============================================================ */
/* 2. 辅助函数 */
/* ============================================================ */

Block* create_block(size_t start, size_t size) {
    Block* b = (Block*)malloc(sizeof(Block));
    b->start = start;
    b->size = size;
    b->next = NULL;
    return b;
}

bool blocks_overlap(Block* b1, Block* b2) {
    size_t s1 = b1->start;
    size_t e1 = s1 + b1->size;
    size_t s2 = b2->start;
    size_t e2 = s2 + b2->size;
    return (s1 < e2 && s2 < e1);
}

/* 将块按地址排序插入到列表 */
void insert_sorted(Block** list, Block* new_block) {
    if (*list == NULL || (*list)->start > new_block->start) {
        new_block->next = *list;
        *list = new_block;
        return;
    }
    Block* current = *list;
    while (current->next != NULL && current->next->start < new_block->start) {
        current = current->next;
    }
    new_block->next = current->next;
    current->next = new_block;
}

/* 合并相邻的空闲块 */
void merge_adjacent(Block* list) {
    if (list == NULL) return;
    Block* current = list;
    while (current->next != NULL) {
        if (current->start + current->size == current->next->start) {
            /* 合并两个块 */
            current->size += current->next->size;
            Block* temp = current->next;
            current->next = temp->next;
            free(temp);
        } else {
            current = current->next;
        }
    }
}

/* ============================================================ */
/* 3. 分配器操作 */
/* ============================================================ */

Allocator* allocator_create(size_t total_size) {
    Allocator* a = (Allocator*)malloc(sizeof(Allocator));
    a->free_list = create_block(0, total_size);
    a->used_list = NULL;
    a->total_size = total_size;
    return a;
}

Block* allocator_alloc(Allocator* a, size_t size) {
    if (size == 0) return NULL;

    Block** current = &a->free_list;
    while (*current != NULL) {
        Block* block = *current;
        if (block->size >= size) {
            Block* allocated = create_block(block->start, size);

            if (block->size == size) {
                /* 精确匹配，移除整个块 */
                *current = block->next;
                free(block);
            } else {
                /* 分割块 */
                block->start += size;
                block->size -= size;
            }

            /* 添加到已用列表 */
            allocated->next = a->used_list;
            a->used_list = allocated;

            return allocated;
        }
        current = &(*current)->next;
    }

    return NULL;  /* 分配失败 */
}

void allocator_free(Allocator* a, Block* block) {
    if (block == NULL || a == NULL) return;

    /* 从已用列表移除 */
    Block** current = &a->used_list;
    while (*current != NULL) {
        if (*current == block) {
            *current = block->next;
            break;
        }
        current = &(*current)->next;
    }

    /* 添加到空闲列表并合并 */
    insert_sorted(&a->free_list, block);
    merge_adjacent(a->free_list);
}

void allocator_destroy(Allocator* a) {
    if (a == NULL) return;

    Block* current = a->free_list;
    while (current != NULL) {
        Block* temp = current;
        current = current->next;
        free(temp);
    }

    current = a->used_list;
    while (current != NULL) {
        Block* temp = current;
        current = current->next;
        free(temp);
    }

    free(a);
}

/* ============================================================ */
/* 4. 测试函数 */
/* ============================================================ */

void test_allocator() {
    Allocator* a = allocator_create(1024);

    /* 测试基本分配 */
    Block* b1 = allocator_alloc(a, 100);
    assert(b1 != NULL);
    assert(b1->size == 100);
    assert(b1->start == 0);

    Block* b2 = allocator_alloc(a, 200);
    assert(b2 != NULL);
    assert(b2->size == 200);
    assert(b2->start == 100);

    /* 测试释放和重用 */
    allocator_free(a, b1);
    Block* b3 = allocator_alloc(a, 50);
    assert(b3 != NULL);
    assert(b3->start == 0);  /* 应该重用释放的块 */

    allocator_destroy(a);
    printf("Allocator tests passed!\n");
}
