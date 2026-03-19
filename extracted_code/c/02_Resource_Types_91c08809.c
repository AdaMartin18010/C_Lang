/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\03_Linear_Logic\02_Resource_Types.md
 * Line: 123
 * Language: c
 * Block ID: 91c08809
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 所有权转移语义
Resource *resource_move(Resource **src) {
    if (!src || !(*src)->valid) {
        return NULL;
    }
    Resource *dst = *src;
    (*src)->valid = false;  // 源变为无效
    *src = NULL;
    return dst;  // 所有权转移
}

// 使用示例
void example(void) {
    Resource *r1 = resource_alloc(100, NULL);

    // 转移所有权
    Resource *r2 = resource_move(&r1);

    // r1现在无效
    // resource_free(r1);  // 运行时错误！

    resource_free(r2);  // 正确
}
