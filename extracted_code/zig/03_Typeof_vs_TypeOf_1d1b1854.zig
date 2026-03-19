//
// Auto-generated from: 08_Zig_C_Connection\06_C23_Zig_Parallel\03_Typeof_vs_TypeOf.md
// Line: 1683
// Language: zig
// Block ID: 1d1b1854
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig @TypeOf 的限制与边界情况

const std = @import("std");

// ========== 限制 1: 运行时值类型推导 ==========

fn runtimeLimitation(x: anytype) void {
    // @TypeOf(x) 在编译时确定，不能反映运行时类型变化
    const T = @TypeOf(x);

    // T 是静态确定的，即使 x 是运行时值
    std.debug.print("Type: {}\n", .{@typeName(T)});
}

// ========== 限制 2: 递归类型推导的复杂性 ==========

const Recursive = struct {
    value: i32,
    // next: ?*Recursive,  // 需要使用指针打破递归
};

// ========== 限制 3: 不透明类型 ==========

extern "C" struct OpaqueType;

fn opaqueLimitation() void {
    var ptr: *OpaqueType = undefined;
    const T = @TypeOf(ptr);  // *OpaqueType
    // 无法知道 OpaqueType 的字段
    _ = T;
}

// ========== 限制 4: 错误联合类型的复杂性 ==========

fn errorUnionLimitation() !void {
    const x = mightFail();
    const T = @TypeOf(x);  // error{...}!i32
    // 需要处理错误联合
    _ = T;
}

fn mightFail() !i32 {
    return 42;
}

// ========== 限制 5: 匿名类型的命名 ==========

fn anonymousTypeLimitation() void {
    var anon = struct {
        x: i32,
        y: i32,
    }{ .x = 1, .y = 2 };

    const T = @TypeOf(anon);
    // T 是匿名结构体类型，无法在其他地方引用

    // 解决方案: 显式命名
    const Named = struct {
        x: i32,
        y: i32,
    };
    var named: Named = .{ .x = 1, .y = 2 };
    _ = named;
}

// ========== 限制 6: 泛型类型推断的边界 ==========

fn genericLimitation(comptime T: type, value: T) void {
    const V = @TypeOf(value);
    // V == T，但 T 可能包含 comptime 信息

    // 如果 T 是 comptime_int，需要具体化
    if (T == comptime_int) {
        // 需要选择具体类型
    }
}

// ========== 限制 7: @TypeOf 在递归函数中的使用 ==========

fn recursiveComptime(comptime n: usize) type {
    if (n == 0) return i32;
    // @TypeOf 在递归类型构造中可能导致复杂性
    return [n]recursiveComptime(n - 1);
}
