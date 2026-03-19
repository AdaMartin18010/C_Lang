/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\11_CompCert_Verification\01_Compcert_Overview.md
 * Line: 248
 * Language: c
 * Block ID: 3698febd
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// CompCert C 支持的特性示例

// 1. 基本数据类型
void basic_types(void) {
    char c;
    short s;
    int i;
    long l;
    long long ll;
    float f;
    double d;
    long double ld;
    _Bool b;  // C99

    // 固定宽度类型
    int32_t i32;
    uint64_t u64;
}

// 2. 复合类型
struct Point {
    int x, y;
};

union Data {
    int i;
    float f;
};

// 3. 指针和数组
void pointer_array(void) {
    int arr[10];
    int* ptr = arr;
    int (*arr_ptr)[10] = &arr;
    int* arr2d[5][10];
}

// 4. 函数
int factorial(int n) {
    if (n <= 1) return 1;
    return n * factorial(n - 1);
}

// 5. 控制流
void control_flow(int x) {
    if (x > 0) {
        // ...
    } else if (x < 0) {
        // ...
    } else {
        // ...
    }

    for (int i = 0; i < 10; i++) {
        if (i == 5) continue;
        if (i == 8) break;
    }

    switch (x) {
        case 1: /* ... */ break;
        case 2: /* ... */ break;
        default: /* ... */
    }
}

// 6. 内存管理
void memory_management(void) {
    int* p = malloc(sizeof(int) * 100);
    if (p) {
        // 使用 p
        free(p);
    }
}

// 7. C11 原子操作 (部分支持)
#include <stdatomic.h>
_Atomic int counter = ATOMIC_VAR_INIT(0);

void atomic_ops(void) {
    atomic_fetch_add(&counter, 1);
}
