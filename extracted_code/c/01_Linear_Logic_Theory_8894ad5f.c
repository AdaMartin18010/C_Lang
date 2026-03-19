/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\03_Linear_Logic\01_Linear_Logic_Theory.md
 * Line: 345
 * Language: c
 * Block ID: 8894ad5f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 积类型的C模拟
// 提供两个投影，但只能使用其中一个
typedef struct {
    Resource *a;
    Resource *b;
    bool a_used;
    bool b_used;
} With;

With *with_create(Resource *a, Resource *b) {
    With *w = malloc(sizeof(With));
    w->a = a;
    w->b = b;
    w->a_used = false;
    w->b_used = false;
    return w;
}

// 投影1：获取A（之后B不可访问）
Resource *with_proj1(With *w) {
    if (w->b_used) {
        // 错误：已经选择了B
        return NULL;
    }
    w->a_used = true;
    Resource *result = w->a;
    // B被丢弃
    resource_destroy(w->b);
    free(w);
    return result;
}

Resource *with_proj2(With *w) {
    // 对称...
}
