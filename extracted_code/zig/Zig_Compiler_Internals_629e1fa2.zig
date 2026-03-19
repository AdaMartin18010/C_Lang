//
// Auto-generated from: Zig\Zig_Compiler_Internals.md
// Line: 176
// Language: zig
// Block ID: 629e1fa2
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 编译时计算阶乘
fn factorial(comptime n: u32) u32 {
    return if (n == 0) 1 else n * factorial(n - 1);
}

const fact5 = factorial(5); // 编译时常量 120

// 类型生成
fn Vector(comptime T: type, comptime n: usize) type {
    return struct {
        data: [n]T,
    };
}
