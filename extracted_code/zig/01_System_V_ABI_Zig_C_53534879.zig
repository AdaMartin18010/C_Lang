//
// Auto-generated from: 08_Zig_C_Connection\03_ABI_Formalization\01_System_V_ABI_Zig_C.md
// Line: 1795
// Language: zig
// Block ID: 53534879
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig 代码: 对齐控制

// 默认对齐的结构体
const DefaultAlign = extern struct {
    c: u8,
    i: i32,
};
// Zig 会自动计算对齐，与 C 默认行为相同

// 显式对齐
const ExplicitAlign = extern struct {
    c: u8,
    // Zig 使用 align 关键字
    i: i32 align(16),
};

// packed 结构体
const PackedStruct = packed struct {
    c: u8,
    i: i32,
};
// 注意: Zig 的 packed struct 位序与 C 可能不同

// 对齐的缓冲区
var cache_line_buffer: [64]u8 align(64) = undefined;

// 在 Zig 中检查和确保对齐
fn ensureAlignment() void {
    comptime {
        // 编译时检查
        if (@alignOf(DefaultAlign) != 4) {
            @compileError("对齐不匹配");
        }
    }

    // 运行时检查
    const ptr: *u8 = &cache_line_buffer[0];
    std.debug.assert(@intFromPtr(ptr) % 64 == 0);
}

// C 兼容的对齐结构体
const AlignedForC = extern struct {
    data: [16]u8 align(16),
};

// 确保与 C 的 alignas(16) char[16] 兼容
comptime {
    assert(@sizeOf(AlignedForC) == 16);
    assert(@alignOf(AlignedForC) == 16);
}
