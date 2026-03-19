//
// Auto-generated from: Zig\README.md
// Line: 819
// Language: zig
// Block ID: 7c86e9ac
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

const c = @cImport({
    @cInclude("stdio.h");
    @cInclude("string.h");
    @cInclude("math.h");
});

const std = @import("std");

pub fn main() !void {
    // 使用 printf
    const name = "Zig";
    c.printf("Hello from %s!\n", name);

    // 使用字符串函数
    const str1 = "hello";
    const str2 = "world";
    const result = c.strcmp(str1, str2);
    std.debug.print("Compare result: {d}\n", .{result});

    // 使用数学函数
    const x: f64 = 2.0;
    const y = c.pow(x, 3.0);
    std.debug.print("2^3 = {d}\n", .{y});
}
