//
// Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\README.md
// Line: 192
// Language: zig
// Block ID: 3b9ab6fb
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

const std = @import("std");

// 确保与 C 结构体完全兼容
const CCompatibleStruct = extern struct {
    x: i32,      // 4字节
    // 隐式填充 4字节
    y: i64,      // 8字节，需8字节对齐
    z: i16,      // 2字节
    // 隐式填充 6字节
}; // 总计 24字节

// 显式控制布局
const PackedStruct = packed struct {
    x: i32,  // offset 0
    y: i64,  // offset 4 (无填充！)
    z: i16,  // offset 12
}; // 总计 14字节

comptime {
    std.debug.assert(@sizeOf(CCompatibleStruct) == 24);
    std.debug.assert(@sizeOf(PackedStruct) == 14);
    std.debug.assert(@alignOf(CCompatibleStruct) == 8);
}
