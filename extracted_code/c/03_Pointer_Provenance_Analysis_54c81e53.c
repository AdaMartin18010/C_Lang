/*
 * Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\03_Pointer_Provenance_Analysis.md
 * Line: 1783
 * Language: c
 * Block ID: 54c81e53
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C 安全实践
#include <stdint.h>
#include <string.h>

// 实践 1：使用 size_t 进行指针算术
void safe_pointer_arithmetic(void) {
    char arr[100];
    size_t offset = 50;
    char* p = arr + offset;  // 安全：char 是字节单位
    *p = 'x';
}

// 实践 2：显式检查边界
void safe_bounds_check(int* arr, size_t len, size_t index) {
    if (index < len) {
        arr[index] = 42;
    }
}

// 实践 3：使用 union 进行类型双关
typedef union {
    uint32_t i;
    float f;
} FloatBits;

void safe_type_punning(void) {
    FloatBits u;
    u.f = 3.14f;
    uint32_t bits = u.i;  // 安全：通过 union
}

// 实践 4：释放后置空
void safe_free(void** pp) {
    free(*pp);
    *pp = NULL;
}

// 实践 5：使用 restrict 帮助优化
void safe_restrict(int* restrict a, int* restrict b, int n) {
    // 告诉编译器 a 和 b 不别名
    for (int i = 0; i < n; i++) {
        a[i] = b[i];
    }
}
