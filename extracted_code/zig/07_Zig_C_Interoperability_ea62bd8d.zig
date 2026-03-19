//
// Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\07_Zig_C_Interoperability.md
// Line: 60
// Language: zig
// Block ID: ea62bd8d
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig vs C 语法对比

// 变量声明
const x: i32 = 10;      // Zig: 不可变
var y: i32 = 20;        // Zig: 可变
// int x = 10;          // C

// 函数
def add(a: i32, b: i32) i32 {
    return a + b;
}
// int add(int a, int b) { return a + b; }  // C

// 数组
const arr = [_]i32{1, 2, 3, 4, 5};
// int arr[] = {1, 2, 3, 4, 5};  // C

// 指针
var p: *i32 = &y;       // 非空指针
var opt: ?*i32 = null;  // 可选指针 (可为null)
// int *p = &y;         // C

// 错误处理
def mightFail() !i32 {
    return error.SomeError;
}
// 没有直接对应，C用返回值或errno
