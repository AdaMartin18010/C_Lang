/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\03_Linear_Logic\01_Linear_Logic_Theory.md
 * Line: 132
 * Language: c
 * Block ID: 111b004f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 张量类型的C模拟
typedef struct {
    Resource *first;
    Resource *second;
    // 两者同时存在，析构时都要释放
} Tensor;

Tensor *tensor_create(Resource *a, Resource *b) {
    Tensor *t = malloc(sizeof(Tensor));
    t->first = a;
    t->second = b;
    return t;
}

void tensor_destroy(Tensor *t) {
    // 必须同时销毁两者
    resource_destroy(t->first);
    resource_destroy(t->second);
    free(t);
}
