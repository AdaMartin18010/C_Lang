/*
 * Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\03_Pointer_Provenance_Analysis.md
 * Line: 1479
 * Language: c
 * Block ID: cb1635dd
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 局部变量地址返回（经典错误）
int* dangling_local(void) {
    int local = 42;
    return &local;  // 返回局部变量地址！
}  // local 生命周期结束，返回的指针来源失效

// 栈使用后被覆盖
void stack_use_after_scope(void) {
    int* p;
    {
        int local = 42;
        p = &local;
    }  // local 生命周期结束
    // *p 可能已被其他栈帧覆盖
}

// 重新分配导致的来源失效
void realloc_invalidation(void) {
    int* p = malloc(10 * sizeof(int));

    // p 可能失效，如果 realloc 移动内存
    int* new_p = realloc(p, 20 * sizeof(int));
    if (new_p) {
        p = new_p;
    }

    // 如果 realloc 失败，p 仍然有效
    // 但无论如何，不能继续使用旧的 p
}
