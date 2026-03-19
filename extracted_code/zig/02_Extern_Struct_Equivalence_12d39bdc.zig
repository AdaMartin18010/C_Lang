//
// Auto-generated from: 08_Zig_C_Connection\01_Type_System_Mapping\02_Extern_Struct_Equivalence.md
// Line: 769
// Language: zig
// Block ID: 12d39bdc
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig 代码 - packed struct
const std = @import("std");

// packed struct: 1 字节对齐，字段紧凑排列
const PackedStruct = packed struct {
    c: u8,
    i: u32,
    s: u16,
};
// @sizeOf = 7
// @alignOf = 1

// packed struct 的字段访问
fn accessPacked() void {
    var p: PackedStruct = .{
        .c = 1,
        .i = 0x12345678,
        .s = 0xABCD,
    };

    // 非对齐字段访问由 Zig 编译器处理
    // 可能生成多字节加载指令或memcpy
    const val = p.i;
    std.debug.print("i = 0x{x}\n", .{val});
}

// 网络协议头 (packed struct 的常见用例)
const TcpHeader = packed struct {
    src_port: u16,
    dst_port: u16,
    seq_num: u32,
    ack_num: u32,
    // 位域模拟
    data_offset: u4,
    reserved: u6,
    flags: u6,
    window_size: u16,
    checksum: u16,
    urgent_ptr: u16,
};

// packed struct 指针的特殊性
fn packedPointerDemo() void {
    var p: PackedStruct = undefined;

    // packed struct 字段的指针是对齐的
    const ptr_to_c: *u8 = &p.c;      // 1-byte aligned

    // 对于非对齐字段，Zig 生成特殊处理
    // 注意: &p.i 会产生警告或错误，因为 i 未对齐

    // 通过指针访问需要使用 @ptrCast 谨慎处理
    const bytes = std.mem.asBytes(&p);
    const i_ptr: *const u32 = @ptrCast(@alignCast(bytes.ptr + 1));
    std.debug.print("i via pointer = 0x{x}\n", .{i_ptr.*});
}

// packed struct 的内存布局
fn printLayout() void {
    std.debug.print("PackedStruct layout:\n", .{});
    std.debug.print("  @sizeOf = {d}\n", .{@sizeOf(PackedStruct)});
    std.debug.print("  @alignOf = {d}\n", .{@alignOf(PackedStruct)});
}
