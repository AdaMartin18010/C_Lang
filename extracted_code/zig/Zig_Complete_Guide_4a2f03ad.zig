//
// Auto-generated from: Zig\Zig_Complete_Guide.md
// Line: 142
// Language: zig
// Block ID: 4a2f03ad
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 原则1: 无隐藏控制流
// 不像 C++ 的构造函数/析构函数，Zig 的一切都是显式的

// 原则2: 无隐藏内存分配
// 每个可能分配内存的操作都必须传递分配器
const std = @import("std");

fn good_example(allocator: std.mem.Allocator) !void {
    const buf = try allocator.alloc(u8, 100);  // 显式分配
    defer allocator.free(buf);                  // 显式释放
    // ...
}

// 原则3: 编译时代码执行
// 利用 comptime 在编译期完成计算
const array_len = blk: {
    var sum: usize = 0;
    for (0..10) |i| {
        sum += i;
    }
    break :blk sum;  // 编译时常量 45
};

// 原则4: 与 C 无缝互操作
const c = @cImport({
    @cInclude("stdio.h");
});

pub export fn zig_hello() void {
    c.printf("Hello from Zig!\n");
}
