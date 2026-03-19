/*
 * Auto-generated from: 08_Zig_C_Connection\03_ABI_Formalization\01_System_V_ABI_Zig_C.md
 * Line: 841
 * Language: c
 * Block ID: cabff7d8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C 代码: 可寄存器传递的小结构体

// 8字节结构体 - 单个 INTEGER 类
struct Small {
    int x;
};
void small_struct(struct Small s) {
    // s.x: %rdi
}

// 16字节结构体 - 两个 INTEGER 类
struct Medium {
    long x, y;
};
void medium_struct(struct Medium s) {
    // s.x: %rdi
    // s.y: %rsi
}

// 含浮点的 16字节结构体
struct Mixed {
    int i;
    float f;
};
void mixed_struct(struct Mixed s) {
    // 分类: word0 = INTEGER (int)
    //       word1 = SSE (float padding)
    // 结果: %rdi, %xmm0
}

// 完全由浮点组成的结构体
struct FloatPair {
    float x, y;
};
void float_pair(struct FloatPair s) {
    // %xmm0 (两个 float 打包)
}

struct DoublePair {
    double x, y;
};
void double_pair(struct DoublePair s) {
    // %xmm0, %xmm1
}
