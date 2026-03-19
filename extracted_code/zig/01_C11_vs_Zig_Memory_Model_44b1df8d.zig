//
// Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\01_C11_vs_Zig_Memory_Model.md
// Line: 979
// Language: zig
// Block ID: 44b1df8d
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig 数据竞争示例
const std = @import("std");

var shared: i32 = 0;  // 非原子变量

fn threadFunc() void {
    // 与 main 的读取构成数据竞争
    shared = 42;  // 写操作
}

pub fn main() !void {
    const t = try std.Thread.spawn(.{}, threadFunc, .{});

    // 与 threadFunc 的写入构成数据竞争
    const value = shared;  // 读操作 - 未定义行为！

    t.join();
    std.debug.print("value: {d}\n", .{value});  // 可能输出任意值
}

// 正确版本 - 使用原子变量
const Atomic = std.atomic.Value;

var shared_atomic = Atomic(i32).init(0);

fn threadFuncSafe() void {
    shared_atomic.store(42, .Release);
}

pub fn mainSafe() !void {
    const t = try std.Thread.spawn(.{}, threadFuncSafe, .{});

    // 使用 acquire 读取，与 release 写入配对
    const value = shared_atomic.load(.Acquire);

    t.join();
    std.debug.print("value: {d}\n", .{value});  // 保证输出 42
}
