/*
 * Auto-generated from: 08_Zig_C_Connection\03_ABI_Formalization\02_Windows_ABI_Compatibility.md
 * Line: 359
 * Language: c
 * Block ID: 83998710
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C 代码: Windows x64 结构体返回

// 8字节结构体 - 直接返回
struct Small {
    int a, b;
};
struct Small return_small(void) {
    struct Small s = {1, 2};
    return s;  // 返回在 RAX 中
}

// 16字节结构体 - 使用 RAX:RDX
struct Medium {
    int64_t a, b;
};
struct Medium return_medium(void) {
    struct Medium m = {1, 2};
    return m;  // a 在 RAX, b 在 RDX
}

// 大结构体 - 使用隐藏指针
struct Large {
    int data[10];
};
struct Large return_large(void) {
    struct Large l = {{0}};
    return l;  // 调用者分配空间，指针通过 RCX 传递
}
// 实际编译为:
// void return_large(struct Large* __hidden_return_ptr);
