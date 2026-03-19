//
// Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\02_Atomic_Operations_Mapping.md
// Line: 766
// Language: zig
// Block ID: ee81ef75
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig Lock-Free 保证
// Zig 保证以下类型总是 lock-free:
// • 所有整数类型 (i8-i128, u8-u128, usize, isize)
// • 布尔类型 (bool)
// • 指针类型 (*T, ?*T)
// • 枚举类型（基于整数）

fn requireLockFree(comptime T: type) void {
    comptime {
        const info = @typeInfo(T);
        const is_lock_free = switch (info) {
            .Int, .Bool => true,
            .Pointer => true,
            .Enum => true,
            else => @sizeOf(T) <= @sizeOf(usize) * 2,
        };
        if (!is_lock_free) {
            @compileError("Type not lock-free");
        }
    }
}
