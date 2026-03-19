//
// Auto-generated from: Zig\Zig_Complete_Guide.md
// Line: 199
// Language: zig
// Block ID: 4815b84c
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// hello.zig
const std = @import("std");

pub fn main() !void {
    // 获取标准输出流
    const stdout = std.io.getStdOut().writer();

    // 打印到 stdout
    try stdout.print("Hello, {s}!\n", .{"World"});

    // 或者使用控制台输出
    std.debug.print("Debug output: {}\n", .{42});
}

// 编译和运行
// zig run hello.zig          # 直接运行
// zig build-exe hello.zig    # 编译可执行文件
// ./hello                    # 运行
