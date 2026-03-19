/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\03_Linear_Logic\02_Resource_Types.md
 * Line: 87
 * Language: c
 * Block ID: 08d3632c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 资源句柄抽象
typedef struct {
    void *ptr;
    size_t size;
    void (*destructor)(void *);
    bool valid;  // 所有权标记
} Resource;

// 资源创建
Resource *resource_alloc(size_t size, void (*dtor)(void *)) {
    Resource *r = malloc(sizeof(Resource));
    r->ptr = malloc(size);
    r->size = size;
    r->destructor = dtor;
    r->valid = true;
    return r;
}

// 资源销毁（消耗所有权）
void resource_free(Resource *r) {
    if (!r || !r->valid) {
        // 双重释放或无效资源
        abort();
    }
    if (r->destructor) {
        r->destructor(r->ptr);
    }
    free(r->ptr);
    r->valid = false;
    free(r);
}
