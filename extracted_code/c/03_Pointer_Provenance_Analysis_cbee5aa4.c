/*
 * Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\03_Pointer_Provenance_Analysis.md
 * Line: 1693
 * Language: c
 * Block ID: cbee5aa4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 陷阱 1：整数转换后丢失来源
void trap_int_conversion(void) {
    int* p = malloc(sizeof(int));
    uintptr_t addr = (uintptr_t)p;
    free(p);

    int* q = (int*)addr;  // 来源已失效！
    // *q = 42;  // Use-After-Free
}

// 陷阱 2：越界指针算术
void trap_oob_arithmetic(void) {
    int arr[10];
    int* p = arr + 100;  // 来源丢失
    // 即使不直接使用，某些比较也是 UB
}

// 陷阱 3：严格别名违反
void trap_strict_aliasing(int* a, float* b) {
    *a = 1;
    *b = 2.0f;
    printf("%d\n", *a);  // 可能输出 1，违反直觉
}

// 陷阱 4： offsetof 误用
struct S { int x; int y; };
void trap_offsetof(void) {
    struct S s;
    int* y_ptr = (int*)&s + offsetof(struct S, y) / sizeof(int);
    // 严格来说，&s 转为 int* 后算术是 UB
    // 正确做法：(char*)&s + offsetof(...)
}

// 陷阱 5：realloc 后继续使用旧指针
void trap_realloc(void) {
    int* p = malloc(10 * sizeof(int));
    int* old_p = p;
    p = realloc(p, 20 * sizeof(int));
    // old_p 可能已失效！
    old_p[0] = 42;  // 危险！
}
