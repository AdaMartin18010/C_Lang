//
// Auto-generated from: 08_Zig_C_Connection\04_Translation_Validation\README.md
// Line: 345
// Language: zig
// Block ID: 08bfaf13
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

test "translation equivalence - string operations" {
    const c = @cImport({
        @cInclude("string_ops.h");
    });

    // 测试输入数据
    const test_cases = .{
        .{ "hello", "HELLO" },
        .{ "World123", "WORLD123" },
        .{ "", "" },
        .{ "MiXeD", "MIXED" },
    };

    inline for (test_cases) |tc| {
        const input = tc[0];
        const expected = tc[1];

        // C 版本
        var c_buffer: [256]u8 = undefined;
        @memcpy(c_buffer[0..input.len], input);
        c_buffer[input.len] = 0;
        c.to_uppercase(&c_buffer);

        // Zig 版本
        var zig_buffer: [256]u8 = undefined;
        @memcpy(zig_buffer[0..input.len], input);
        toUppercase(zig_buffer[0..input.len]);

        // 验证等价性
        try std.testing.expectEqualStrings(
            c_buffer[0..input.len :0],  // 以null结尾的切片
            zig_buffer[0..input.len]
        );
        try std.testing.expectEqualStrings(
            expected,
            zig_buffer[0..input.len]
        );
    }
}

fn toUppercase(s: []u8) void {
    for (s) |*c| {
        c.* = std.ascii.toUpper(c.*);
    }
}
