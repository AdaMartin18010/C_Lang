//
// Auto-generated from: 08_Zig_C_Connection\06_C23_Zig_Parallel\02_Constexpr_vs_Comptime.md
// Line: 245
// Language: zig
// Block ID: 68416b09
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig: comptime 函数

// 基本 comptime 函数（参数标记）
fn square(comptime x: i32) i32 {
    return x * x;
}

// 递归 comptime 函数
fn factorial(comptime n: u32) u32 {
    return if (n <= 1) 1 else n * factorial(n - 1);
}

// 迭代版本（同样支持）
fn fibonacci(comptime n: u32) u32 {
    if (n <= 1) return n;
    var a: u32 = 0;
    var b: u32 = 1;
    var i: u32 = 2;
    while (i <= n) : (i += 1) {
        const temp = a + b;
        a = b;
        b = temp;
    }
    return b;
}

// 显式 comptime 调用
const FIB_10 = comptime fibonacci(10);
var fib_array: [FIB_10]i32 = undefined;

// 类型参数函数（Zig 独有）
fn max(comptime T: type, a: T, b: T) T {
    return if (a > b) a else b;
}

const max_int = max(i32, 10, 20);  // 20
const max_float = max(f64, 3.14, 2.71);  // 3.14
