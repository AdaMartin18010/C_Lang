/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\03_Linear_Logic\02_Resource_Types.md
 * Line: 296
 * Language: c
 * Block ID: 732c5225
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// RAII模式
typedef struct {
    Resource *r;
    void (*cleanup)(Resource *);
} ScopedResource;

#define SCOPED(name, alloc_expr) \
    ScopedResource name __attribute__((cleanup(scoped_cleanup))) = { \
        .r = (alloc_expr), \
        .cleanup = resource_free \
    }

void scoped_cleanup(ScopedResource *sr) {
    if (sr->cleanup && sr->r) {
        sr->cleanup(sr->r);
    }
}

// 使用示例
void scoped_example(void) {
    SCOPED(file, fopen("test.txt", "r"));
    // 使用file.r...
    // 函数返回时自动关闭
}
