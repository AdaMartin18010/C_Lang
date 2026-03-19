//
// Auto-generated from: Zig\Zig_Master_Guide.md
// Line: 200
// Language: zig
// Block ID: 8a1f9d5f
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// hello.zig
const std = @import("std");

pub fn main() !void {
    const stdout = std.io.getStdOut().writer();
    try stdout.print("Hello, {s}!\n", .{"Zig"});
}
