//
// Auto-generated from: Zig\Zig_OS_Development.md
// Line: 423
// Language: zig
// Block ID: 85223ddb
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// vmm.zig - 虚拟内存管理
const std = @import("std");

// 页表项标志
pub const PageFlags = packed struct {
    present: bool = true,
    writable: bool = true,
    user: bool = false,
    accessed: bool = false,
    dirty: bool = false,

    pub fn toU64(self: PageFlags) u64 {
        return @as(u64, @bitCast(self));
    }
};

pub const PageTable = extern struct {
    entries: [512]u64 align(4096),

    pub fn init(table: *PageTable) void {
        @memset(&table.entries, 0);
    }

    pub fn mapPage(
        self: *PageTable,
        virtual: usize,
        physical: usize,
        flags: PageFlags,
    ) void {
        const p4_index = (virtual >> 39) & 0x1FF;
        const p3_index = (virtual >> 30) & 0x1FF;
        const p2_index = (virtual >> 21) & 0x1FF;
        const p1_index = (virtual >> 12) & 0x1FF;

        // 简化实现：假设页表已分配
        _ = p4_index;
        _ = p3_index;
        _ = p2_index;

        // 获取或创建 P1 页表
        const p1 = getP1Table(self, virtual);
        p1.entries[p1_index] = (physical & 0x000FFFFFFFFFF000) | flags.toU64();
    }
};

pub fn flushTlb(virtual: usize) void {
    asm volatile ("invlpg (%[addr])"
        :
        : [addr] "r" (virtual),
        : "memory"
    );
}
