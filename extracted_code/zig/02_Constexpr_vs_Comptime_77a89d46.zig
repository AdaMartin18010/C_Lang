//
// Auto-generated from: 08_Zig_C_Connection\06_C23_Zig_Parallel\02_Constexpr_vs_Comptime.md
// Line: 1171
// Language: zig
// Block ID: 77a89d46
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig: comptime 递归限制
const std = @import("std");

// 递归 comptime 函数
fn recursive_add(comptime n: i32) i32 {
    return if (n <= 0) 0 else n + recursive_add(n - 1);
}

// Zig 的递归限制由可用内存决定，而非固定层数
const SUM_1000 = recursive_add(1000);  // OK
const SUM_10000 = recursive_add(10000);  // 可能 OK，取决于内存

// 尾递归 - Zig 解释器优化
fn factorial(comptime n: u32, comptime acc: u32) u32 {
    return if (n <= 1) acc else factorial(n - 1, n * acc);
}

const FACT_20 = factorial(20, 1);  // 2432902008176640000

// 斐波那契 - 指数级
fn fib(comptime n: u32) u32 {
    return if (n <= 1) n else fib(n - 1) + fib(n - 2);
}

// 计算复杂度问题，而非递归深度问题
// const FIB_50 = fib(50);  // 编译时间极长（指数级调用）

// 解决方案 1：迭代版本
fn fib_iter(comptime n: u32) u32 {
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

const FIB_100 = fib_iter(100);  // 快速编译

// 解决方案 2：记忆化（comptime 哈希表）
fn fib_memoized(comptime n: u32) u32 {
    comptime {
        var memo: [101]u32 = undefined;
        memo[0] = 0;
        memo[1] = 1;
        var i: usize = 2;
        while (i <= n) : (i += 1) {
            memo[i] = memo[i - 1] + memo[i - 2];
        }
        return memo[n];
    }
}

const FIB_100_MEMO = fib_memoized(100);  // 线性时间
