/*
 * Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\03_Pointer_Provenance_Analysis.md
 * Line: 602
 * Language: c
 * Block ID: 02023e7e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 违反严格别名的危险示例
#include <stdio.h>

// 编译器使用 -fstrict-aliasing（默认启用）优化
int dangerous_optimization(int* a, int* b) {
    *a = 1;
    *b = 2;
    return *a;  // 编译器可能假设 b 不别名 a
}

void test_alias(void) {
    int x;
    // 用相同地址调用
    int result = dangerous_optimization(&x, &x);
    // 期望：result = 2（因为 b 写入覆盖）
    // 实际：可能 result = 1（优化后）
    printf("result: %d\n", result);
}

// 更隐蔽的例子
float global_f;

int alias_through_globals(int* ip) {
    global_f = 1.0f;  // 写入 float
    *ip = 0x3f800000;  // 1.0f 的 int 表示
    return global_f > 0.5f;  // 编译器可能假设 ip 不别名 global_f
}
