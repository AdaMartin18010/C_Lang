/*
 * Auto-generated from: 08_Zig_C_Connection\03_ABI_Formalization\01_System_V_ABI_Zig_C.md
 * Line: 788
 * Language: c
 * Block ID: 67e397cb
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C 代码: 内存传递的结构体

// 大于 16 字节的结构体 - 通过内存传递
struct BigStruct {
    long a, b, c;    // 24 bytes
};
void big_struct(struct BigStruct s) {
    // s 通过栈传递 (复制 24 字节)
}

// 含有未对齐字段的结构体 - 内存传递
struct Unaligned {
    char c;
    __attribute__((packed)) int i;  // 未对齐的 int
};
void unaligned_struct(struct Unaligned s) {
    // MEMORY 类
}

// 通过指针传递 (更高效的替代方案)
void big_struct_ptr(const struct BigStruct* s) {
    // s: %rdi (8字节指针)
}
