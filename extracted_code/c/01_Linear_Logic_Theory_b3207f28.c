/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\03_Linear_Logic\01_Linear_Logic_Theory.md
 * Line: 393
 * Language: c
 * Block ID: b3207f28
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 指数类型的C模拟
// !A 实现为引用计数或垃圾回收
typedef struct {
    Resource *inner;
    int refcount;
} Exponential;

Exponential *exp_create(Resource *r) {
    Exponential *e = malloc(sizeof(Exponential));
    e->inner = r;
    e->refcount = 1;
    return e;
}

// 复制（dereliction + contraction）
Exponential *exp_copy(Exponential *e) {
    e->refcount++;
    return e;
}

// 丢弃（weakening）
void exp_discard(Exponential *e) {
    e->refcount--;
    if (e->refcount == 0) {
        resource_destroy(e->inner);
        free(e);
    }
}

// 使用（dereliction）
Resource *exp_use(Exponential *e) {
    // 返回内部资源，但不转移所有权
    return e->inner;
}
