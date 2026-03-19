/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\04_Secure_Coding_Guide.md
 * Line: 504
 * Language: c
 * Block ID: ff8d3502
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// UAF检测模式
#define UAF_SENTINEL 0xDEADBEEFCAFEBABEULL

// 带哨兵的内存结构
typedef struct {
    uint64_t sentinel;
    size_t size;
    char data[];  // 柔性数组
} guarded_block_t;

void *guarded_alloc(size_t size) {
    guarded_block_t *block = malloc(sizeof(guarded_block_t) + size);
    if (!block) return NULL;
    
    block->sentinel = UAF_SENTINEL;
    block->size = size;
    
    return block->data;
}

void guarded_free(void *ptr) {
    if (!ptr) return;
    
    guarded_block_t *block = (void *)((char *)ptr - sizeof(guarded_block_t));
    
    // 验证哨兵
    if (block->sentinel != UAF_SENTINEL) {
        fprintf(stderr, "[FATAL] Invalid free or heap corruption\n");
        abort();
    }
    
    // 毒化内存
    memset(block->data, 0xDD, block->size);  // 0xDD = dead
    block->sentinel = 0xBADC0DE;
    
    free(block);
}

// 智能指针模拟（简化版）
typedef struct {
    void *ptr;
    bool owned;
} smart_ptr_t;

smart_ptr_t make_smart(void *p) {
    return (smart_ptr_t){.ptr = p, .owned = true};
}

void smart_free(smart_ptr_t *sp) {
    if (sp && sp->owned) {
        free(sp->ptr);
        sp->ptr = NULL;
        sp->owned = false;
    }
}

// 使用RAII模式
#define SCOPE_PTR(type, name, init) \
    type name __attribute__((cleanup(smart_cleanup))) = (init)
