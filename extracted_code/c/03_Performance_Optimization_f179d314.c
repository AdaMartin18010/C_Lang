/*
 * Auto-generated from: 01_Core_Knowledge_System\05_Engineering_Layer\03_Performance_Optimization.md
 * Line: 130
 * Language: c
 * Block ID: f179d314
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 结构体字段排序（从大到小）
// ✅ 优化布局
struct Optimized {
    double d;      // 8字节
    int i;         // 4字节
    short s;       // 2字节
    char c;        // 1字节
    char pad;      // 填充到16字节
};

// 池化分配
typedef struct {
    char *pool;
    size_t used;
    size_t size;
} MemoryPool;

void *pool_alloc(MemoryPool *p, size_t n) {
    if (p->used + n > p->size) return NULL;
    void *ptr = p->pool + p->used;
    p->used += n;
    return ptr;
}
