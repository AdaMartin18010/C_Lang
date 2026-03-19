//
// Auto-generated from: Zig\Zig_OS_Development.md
// Line: 909
// Language: zig
// Block ID: ce0762a7
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// elf.zig - ELF 文件加载
const std = @import("std");

pub const Elf64Header = packed struct {
    ident: [16]u8,
    type: u16,
    machine: u16,
    version: u32,
    entry: u64,
    phoff: u64,
    shoff: u64,
    flags: u32,
    ehsize: u16,
    phentsize: u16,
    phnum: u16,
    shentsize: u16,
    shnum: u16,
    shstrndx: u16,
};

pub const ProgramHeader = packed struct {
    type: u32,
    flags: u32,
    offset: u64,
    vaddr: u64,
    paddr: u64,
    filesz: u64,
    memsz: u64,
    align: u64,
};

pub fn loadElf(data: []const u8, page_table: anytype) !u64 {
    const header = @as(*const Elf64Header, @ptrCast(data.ptr));

    // 验证魔数
    if (!std.mem.eql(u8, header.ident[0..4], &.{ 0x7F, 'E', 'L', 'F' })) {
        return error.InvalidElf;
    }

    // 加载程序段
    var ph_offset: usize = header.phoff;
    for (0..header.phnum) |_| {
        const ph = @as(*const ProgramHeader, @ptrCast(data.ptr + ph_offset));

        if (ph.type == 1) { // PT_LOAD
            // 映射段到内存
            _ = page_table;
        }

        ph_offset += header.phentsize;
    }

    return header.entry;
}
