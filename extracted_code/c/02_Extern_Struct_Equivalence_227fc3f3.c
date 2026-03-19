/*
 * Auto-generated from: 08_Zig_C_Connection\01_Type_System_Mapping\02_Extern_Struct_Equivalence.md
 * Line: 526
 * Language: c
 * Block ID: 227fc3f3
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C 代码 - 位域声明与语义
#include <stdio.h>

struct BitField {
    unsigned int a : 4;   // 4 bits
    unsigned int b : 8;   // 8 bits
    unsigned int c : 12;  // 12 bits
    unsigned int d : 8;   // 8 bits (跨 int 边界?)
};

// 可能的内存布局 (实现定义!):
// 方案 A (紧凑): [aaaa bbbb bbbb cccc cccc cccc dddd dddd] = 4 bytes
// 方案 B (对齐): [aaaa bbbb bbbb cccc cccc cccc .... ....]
//                [dddd dddd .... .... .... .... .... ....] = 8 bytes

// 有符号位域
struct SignedBitField {
    int x : 3;  // -4 到 3
    int y : 3;  // -4 到 3
};

// 匿名位域 (用于对齐)
struct WithGap {
    unsigned int a : 4;
    unsigned int   : 4;  // 4 bits padding
    unsigned int b : 8;
};

// 零宽度位域 (强制对齐到下一个单元)
struct ZeroWidth {
    unsigned int a : 4;
    unsigned int   : 0;  // 强制填充到 int 边界
    unsigned int b : 4;  // 从新 int 开始
};

int main() {
    printf("sizeof(BitField) = %zu\n", sizeof(struct BitField));

    struct BitField bf = {10, 200, 3000, 128};
    printf("bf.a = %u, bf.b = %u, bf.c = %u, bf.d = %u\n",
           bf.a, bf.b, bf.c, bf.d);
    return 0;
}
