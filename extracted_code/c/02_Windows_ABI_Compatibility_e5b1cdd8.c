/*
 * Auto-generated from: 08_Zig_C_Connection\03_ABI_Formalization\02_Windows_ABI_Compatibility.md
 * Line: 284
 * Language: c
 * Block ID: e5b1cdd8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C 代码: Windows x64 结构体传递

// 8字节整数结构体 - 使用 RCX
struct IntPair {
    int a, b;
};
void pass_int_pair(struct IntPair p) {
    // p: RCX (打包两个32位整数)
}

// 8字节浮点结构体 - 使用 XMM0
struct FloatPair {
    float a, b;
};
void pass_float_pair(struct FloatPair p) {
    // p: XMM0 (打包两个float)
}

// 混合类型结构体 - 必须使用栈或指针
struct Mixed {
    int i;
    float f;
};
void pass_mixed(struct Mixed p) {
    // Windows: 复制到栈 (Shadow Space 之后)
    // 或更常见的做法：传递指针
}

// 大结构体 - 传递指针
struct BigStruct {
    int data[10];
};
void pass_big(struct BigStruct p) {
    // Windows: 栈上复制
    // 推荐：void pass_big(const struct BigStruct* p)
}
