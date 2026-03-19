/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\11_CompCert_Verification\02_VST_Separation_Logic_Practical.md
 * Line: 331
 * Language: c
 * Block ID: 48cfb78f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 简单的块分配器
typedef struct Block {
    size_t size;
    int free;
    struct Block *next;
} Block;

static Block *free_list = NULL;

void *my_malloc(size_t size) {
    Block *curr = free_list;
    while (curr != NULL) {
        if (curr->free && curr->size >= size) {
            curr->free = 0;
            return (void*)(curr + 1);
        }
        curr = curr->next;
    }
    // 分配新块
    Block *block = sbrk(sizeof(Block) + size);
    block->size = size;
    block->free = 0;
    block->next = free_list;
    free_list = block;
    return (void*)(block + 1);
}
