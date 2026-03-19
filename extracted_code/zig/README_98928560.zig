//
// Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\README.md
// Line: 220
// Language: zig
// Block ID: 98928560
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

/// 验证 extern struct 与 C 的兼容性
test "struct layout verification" {
    const c = @cImport({
        @cDefine("VERIFY_STRUCT", "");
        @cInclude("test_struct.h");
    });

    const ZigStruct = extern struct {
        a: u8,
        b: u16,
        c: u32,
    };

    // 验证大小和对齐
    try std.testing.expectEqual(@sizeOf(ZigStruct), @sizeOf(c.TestStruct));
    try std.testing.expectEqual(@alignOf(ZigStruct), @alignOf(c.TestStruct));

    // 验证字段偏移
    try std.testing.expectEqual(
        @offsetOf(ZigStruct, "a"),
        c.offsetof_a
    );
    try std.testing.expectEqual(
        @offsetOf(ZigStruct, "b"),
        c.offsetof_b
    );
    try std.testing.expectEqual(
        @offsetOf(ZigStruct, "c"),
        c.offsetof_c
    );
}
