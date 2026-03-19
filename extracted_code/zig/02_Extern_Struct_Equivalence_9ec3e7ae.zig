//
// Auto-generated from: 08_Zig_C_Connection\01_Type_System_Mapping\02_Extern_Struct_Equivalence.md
// Line: 2046
// Language: zig
// Block ID: 9ec3e7ae
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig 代码 - complex_struct.zig
const std = @import("std");

// 配置值 - 使用 extern union
const ConfigValue = extern struct {
    type: u32,
    value: extern union {
        i: i64,
        f: f64,
        s: extern struct {
            data: [*c]const u8,
            len: usize,
        },
        p: extern struct {
            ptr: ?*anyopaque,
            destructor: ?*const fn (?*anyopaque) callconv(.C) void,
        },
    },
};

// 网络包头 - packed struct
const PacketHeader = packed struct {
    version: u8,
    type: u8,
    length: u16,
    seq: u32,
    timestamp: u64,
};

// 分配器统计
const AllocatorStats = extern struct {
    used: extern struct {
        current: usize,
        peak: usize,
        total: usize,
    },
    blocks: extern struct {
        current: usize,
        total: usize,
    },
    alloc_count: u32,
    free_count: u32,
};

// 时间戳结构
const Timespec = extern struct {
    sec: i64,
    nsec: i32,
};

// 文件元数据
const FileMetadata = extern struct {
    inode: u64,
    size: u64,
    atime: Timespec,
    mtime: Timespec,
    ctime: Timespec,
    mode: u32,
    uid: u32,
    gid: u32,
    links: u32,
};

comptime {
    // 验证大小
    std.debug.assert(@sizeOf(ConfigValue) == 24);
    std.debug.assert(@sizeOf(PacketHeader) == 16);
    std.debug.assert(@sizeOf(AllocatorStats) == 48);
    std.debug.assert(@sizeOf(FileMetadata) == 72);
}

pub fn main() void {
    std.debug.print("=== Complex Struct Layouts ===\n\n", .{});

    std.debug.print("ConfigValue:\n", .{});
    std.debug.print("  size={d}, align={d}\n", .{@sizeOf(ConfigValue), @alignOf(ConfigValue)});
    std.debug.print("  offset(value)={d}\n\n", .{@offsetOf(ConfigValue, "value")});

    std.debug.print("PacketHeader (packed):\n", .{});
    std.debug.print("  size={d}, align={d}\n", .{@sizeOf(PacketHeader), @alignOf(PacketHeader)});

    std.debug.print("\nAllocatorStats:\n", .{});
    std.debug.print("  size={d}, align={d}\n", .{@sizeOf(AllocatorStats), @alignOf(AllocatorStats)});

    std.debug.print("\nFileMetadata:\n", .{});
    std.debug.print("  size={d}, align={d}\n", .{@sizeOf(FileMetadata), @alignOf(FileMetadata)});
}
