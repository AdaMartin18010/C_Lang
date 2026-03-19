/*
 * Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\03_Pointer_Provenance_Analysis.md
 * Line: 199
 * Language: c
 * Block ID: 82713bc6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C99 有效类型示例
#include <string.h>
#include <stdint.h>

union U {
    int i;
    float f;
};

void effective_type_examples(void) {
    // 示例 1: 合法的类型双关 (union)
    union U u;
    u.i = 42;
    float f = u.f;  // OK: 通过 union 访问

    // 示例 2: 合法的字节访问
    int x = 0x12345678;
    unsigned char* bytes = (unsigned char*)&x;
    unsigned char b0 = bytes[0];  // OK: char* 可以访问任何对象

    // 示例 3: 合法的 memcpy
    int src = 42;
    float dst;
    memcpy(&dst, &src, sizeof(int));  // OK: 复制字节

    // 示例 4: malloc 内存的多重类型使用
    void* mem = malloc(sizeof(int));
    int* ip = mem;
    *ip = 42;  // 设置有效类型为 int

    // float* fp = mem;           // 危险！
    // *fp = 3.14f;  // 未定义行为：违反有效类型规则

    free(mem);
}
