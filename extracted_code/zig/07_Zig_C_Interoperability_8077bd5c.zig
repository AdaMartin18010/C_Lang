//
// Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\07_Zig_C_Interoperability.md
// Line: 763
// Language: zig
// Block ID: 8077bd5c
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

const std = @import("std");
const c = @cImport({
    @cInclude("my_c_lib.h");
});

// 测试C函数
test "C library functions" {
    const result = c.c_function(10, 20);
    try std.testing.expectEqual(result, 30);
}

// 测试Zig包装的C代码
fn safe_wrapper(x: i32) !i32 {
    if (x < 0) return error.InvalidInput;
    return c.process_positive(x);
}

test "safe wrapper" {
    try std.testing.expectEqual(safe_wrapper(10), 100);
    try std.testing.expectError(error.InvalidInput, safe_wrapper(-5));
}
