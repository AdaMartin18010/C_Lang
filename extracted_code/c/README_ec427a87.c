/*
 * Auto-generated from: 06_Thinking_Representation\05_Concept_Mappings\README.md
 * Line: 495
 * Language: c
 * Block ID: ec427a87
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 函数组合器
typedef int (*unary_fn_t)(int);

typedef struct {
    unary_fn_t first;
    unary_fn_t second;
} composed_fn_t;

int composed_apply(void* ctx, int x) {
    composed_fn_t* comp = (composed_fn_t*)ctx;
    return comp->second(comp->first(x));
}

// 简化版：直接组合
int compose_int(unary_fn_t f, unary_fn_t g, int x) {
    return g(f(x));
}

// 使用: compose(square, double, 3) => double(square(3)) => 18
