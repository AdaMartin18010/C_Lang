//
// Auto-generated from: Zig\Zig_OS_Development.md
// Line: 231
// Language: zig
// Block ID: d2f5ec04
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// multiboot2.zig - 解析启动信息
const std = @import("std");

pub const Info = struct {
    total_size: u32,
    reserved: u32,

    pub fn iterateTags(self: *const Info) TagIterator {
        return .{
            .current = @as([*]const u8, @ptrCast(self)) + 8,
            .end = @as([*]const u8, @ptrCast(self)) + self.total_size,
        };
    }
};

pub const TagIterator = struct {
    current: [*]const u8,
    end: [*]const u8,

    pub fn next(self: *TagIterator) ?*const Tag {
        if (self.current >= self.end) return null;

        const tag = @as(*const Tag, @ptrCast(self.current));
        if (tag.type == 0) return null;

        // 对齐到 8 字节
        const size = (tag.size + 7) & ~@as(u32, 7);
        self.current += size;

        return tag;
    }
};

pub const Tag = packed struct {
    type: u32,
    size: u32,
};

// 内核主入口
export fn kernel_main(magic: u32, info: *const Info) callconv(.C) noreturn {
    // 验证 multiboot2 magic
    if (magic != 0x36d76289) {
        @panic("Invalid multiboot2 magic!");
    }

    // 解析内存映射
    var iter = info.iterateTags();
    while (iter.next()) |tag| {
        switch (tag.type) {
            6 => { // Memory map
                initMemory(@as(*MemoryMapTag, @ptrCast(tag)));
            },
            9 => { // ELF sections
                // 解析内核符号表
            },
            else => {},
        }
    }

    while (true) {
        asm volatile ("hlt");
    }
}
