//
// Auto-generated from: Zig\Zig_OS_Development.md
// Line: 342
// Language: zig
// Block ID: 1e7876b5
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// pmm.zig - 物理内存管理器
const std = @import("std");

const PAGE_SIZE = 4096;

// 位图分配器
pub const BitmapAllocator = struct {
    bitmap: []u32,
    total_pages: usize,
    used_pages: usize = 0,

    pub fn allocPage(self: *BitmapAllocator) ?usize {
        for (0..self.total_pages) |page| {
            const idx = page / 32;
            const bit = @as(u5, @intCast(page % 32));
            const mask = @as(u32, 1) << bit;

            if ((self.bitmap[idx] & mask) == 0) {
                self.bitmap[idx] |= mask;
                self.used_pages += 1;
                return page * PAGE_SIZE;
            }
        }
        return null;
    }

    pub fn freePage(self: *BitmapAllocator, addr: usize) void {
        const page = addr / PAGE_SIZE;
        const idx = page / 32;
        const bit = @as(u5, @intCast(page % 32));
        self.bitmap[idx] &= ~(@as(u32, 1) << bit);
        self.used_pages -= 1;
    }
};

// 伙伴系统分配器
pub const BuddyAllocator = struct {
    const MIN_BLOCK_SIZE = PAGE_SIZE;
    const MAX_ORDER = 20;

    free_lists: [MAX_ORDER + 1] ?*Block,
    base_addr: usize,

    const Block = struct {
        next: ?*Block,
        order: u8,
    };

    pub fn alloc(self: *BuddyAllocator, size: usize) ?usize {
        const order = calculateOrder(size);
        if (order > MAX_ORDER) return null;

        var current_order = order;
        while (current_order <= MAX_ORDER) : (current_order += 1) {
            if (self.free_lists[current_order]) |block| {
                self.free_lists[current_order] = block.next;

                // 分裂大块
                while (current_order > order) {
                    current_order -= 1;
                    const buddy = @as(*Block, @ptrCast(
                        @as([*]u8, @ptrCast(block)) + (MIN_BLOCK_SIZE << current_order)
                    ));
                    buddy.* = .{
                        .next = self.free_lists[current_order],
                        .order = current_order,
                    };
                    self.free_lists[current_order] = buddy;
                }

                return @intFromPtr(block) - self.base_addr;
            }
        }
        return null;
    }
};
