/*
 * Auto-generated from: 01_Core_Knowledge_System\02_Core_Layer\04_Functions_Scope.md
 * Line: 313
 * Language: c
 * Block ID: 9d8fcf1d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 错误！
int *get_local(void) {
    int local = 42;
    return &local;  // 返回悬空指针！
}

// 正确：使用static或malloc
int *get_static(void) {
    static int static_var = 42;
    return &static_var;  // OK，静态存储期
}

int *get_dynamic(void) {
    int *ptr = malloc(sizeof(int));
    *ptr = 42;
    return ptr;  // OK，但调用者需要free
}
