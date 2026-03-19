//
// Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\02_Atomic_Operations_Mapping.md
// Line: 219
// Language: zig
// Block ID: bd337ac0
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig 复杂原子类型
const Atomic = std.atomic.Value;

// 枚举原子类型
const Color = enum(i32) { red, green, blue };
var current_color = Atomic(Color).init(.red);

// 位标志
var atomic_flags = Atomic(u64).init(0);
fn setFlag(flag: u6) void {
    _ = atomic_flags.fetchOr(@as(u64, 1) << flag, .Monotonic);
}

// 打包原子操作
const PackedFlags = packed struct(u32) {
    status: u8,
    counter: u16,
    flags: u8,
};
var packed_atomic = Atomic(u32).init(0);
