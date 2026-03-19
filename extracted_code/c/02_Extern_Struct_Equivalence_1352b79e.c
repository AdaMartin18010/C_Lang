/*
 * Auto-generated from: 08_Zig_C_Connection\01_Type_System_Mapping\02_Extern_Struct_Equivalence.md
 * Line: 337
 * Language: c
 * Block ID: 1352b79e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C 代码 - 填充计算示例
#include <stdio.h>

// 示例 1: 自然填充
struct Padded {
    char  a;  // 1 byte + 3 bytes padding
    int   b;  // 4 bytes
    char  c;  // 1 byte + 3 bytes padding
};
// 预期: sizeof = 12 (非最优)

// 示例 2: 重排优化
struct Optimized {
    int   b;  // 4 bytes
    char  a;  // 1 byte
    char  c;  // 1 byte + 2 bytes padding
};
// 预期: sizeof = 8 (更优)

// 示例 3: 混合类型
struct Mixed {
    char   a;  // 1 + 7 padding
    double d;  // 8
    char   b;  // 1 + 3 padding
    int    i;  // 4
};
// 预期: sizeof = 24

int main() {
    printf("sizeof(Padded)    = %zu\n", sizeof(struct Padded));
    printf("sizeof(Optimized) = %zu\n", sizeof(struct Optimized));
    printf("sizeof(Mixed)     = %zu\n", sizeof(struct Mixed));
    return 0;
}
