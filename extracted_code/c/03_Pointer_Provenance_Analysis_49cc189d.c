/*
 * Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\03_Pointer_Provenance_Analysis.md
 * Line: 519
 * Language: c
 * Block ID: 49cc189d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C 严格别名规则示例
#include <stdint.h>

void strict_aliasing_violation(int* ip, float* fp) {
    // 违反严格别名规则！
    *ip = 42;
    *fp = 3.14f;  // 编译器假设 ip 和 fp 不别名
    // 可能优化为忽略 ip 的写入！
}

// 正确的做法：使用 union
typedef union {
    int i;
    float f;
} IntFloat;

void correct_type_punning(IntFloat* u) {
    u->i = 42;
    float f = u->f;  // OK: 通过 union 访问
    (void)f;
}

// 或者使用 memcpy
void correct_memcpy(int* ip, float* fp) {
    int value = *ip;
    memcpy(fp, &value, sizeof(int));  // OK: 复制字节
}

// 编译器优化示例
int optimize_me(int* a, long* b) {
    *a = 1;
    *b = 2;
    return *a;  // 编译器可能返回 1，假设 a 和 b 不别名
}
