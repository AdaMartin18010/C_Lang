//
// Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\03_Pointer_Provenance_Analysis.md
// Line: 136
// Language: zig
// Block ID: f2c5badc
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig 的显式来源追踪
const std = @import("std");

fn provenanceBasics() !void {
    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    defer _ = gpa.deinit();
    const allocator = gpa.allocator();

    // 分配获得带来源的切片
    const slice = try allocator.alloc(i32, 10);
    defer allocator.free(slice);
    // slice.ptr 携带来源：指向此分配的 40 字节

    // 同一来源的派生
    const q: [*]i32 = slice.ptr + 5;  // 同一来源
    _ = q;

    // Zig 强烈不推荐整数↔指针转换
    // 如果必须，需要使用 @ptrToInt 和 @intToPtr
    const addr = @ptrToInt(slice.ptr);  // 显式转换
    const r = @intToPtr([*]i32, addr);  // 显式恢复
    _ = r;

    // Zig 的类型系统防止严格别名违规
    // const f: [*]f32 = @ptrCast(slice.ptr);  // 需要显式 @ptrCast
    // f[0] = 3.14;  // 可能违反严格别名，需要小心
}
