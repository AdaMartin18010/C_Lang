/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\03_Linear_Logic\02_Resource_Types.md
 * Line: 515
 * Language: c
 * Block ID: 70a584b1
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 错误：返回局部变量的引用
int *bad_return(void) {
    int local = 42;
    return &local;  // 悬垂指针！
}

// 错误：借用超出生命周期
const int *bad_borrow(void) {
    int local = 42;
    return &local;  // 同样问题
}

// 正确：使用堆分配或延长生命周期
int *good_return(void) {
    int *heap = malloc(sizeof(int));
    *heap = 42;
    return heap;
}
