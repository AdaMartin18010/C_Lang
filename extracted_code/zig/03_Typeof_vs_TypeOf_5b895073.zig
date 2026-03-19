//
// Auto-generated from: 08_Zig_C_Connection\06_C23_Zig_Parallel\03_Typeof_vs_TypeOf.md
// Line: 2629
// Language: zig
// Block ID: 5b895073
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig: 运行时性能分析

const std = @import("std");

// @TypeOf 不影响运行时性能，它只是编译时类型推导

fn zigRuntimePerformance() void {
    // 这两者在运行时性能上完全相同:
    var x1: i32 = 42;
    const x2: @TypeOf(x1) = 42;

    // 泛型函数在编译时实例化，无运行时开销
    const m = max(10, 20);
    // 编译后等同于特定类型的函数调用
}

// comptime 计算完全在编译时完成
fn comptimePerformance() void {
    const table = comptime blk: {
        var t: [256]u8 = undefined;
        for (0..256) |i| {
            t[i] = @truncate(i * 2);
        }
        break :blk t;
    };

    // table 在运行时是一个普通的静态数组
    // 查找是 O(1) 的数组访问
    const val = table[42];
    _ = val;
}
