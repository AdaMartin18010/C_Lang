//
// Auto-generated from: Zig\Zig_OS_Development.md
// Line: 480
// Language: zig
// Block ID: 1d7c1dab
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// heap.zig - 内核堆分配器
const std = @import("std");

// Slab 分配器
pub const SlabAllocator = struct {
    slabs: [8]Slab,

    const Slab = struct {
        object_size: usize,
        free_list: ?*SlabObject,
    };

    const SlabObject = struct {
        next: ?*SlabObject,
    };

    pub fn alloc(self: *SlabAllocator, size: usize) ?[*]u8 {
        const slab_idx = findSlabIndex(size);
        if (slab_idx >= 8) return null;

        var slab = &self.slabs[slab_idx];
        const obj = slab.free_list.?;
        slab.free_list = obj.next;

        return @as([*]u8, @ptrCast(obj));
    }

    fn findSlabIndex(size: usize) usize {
        var idx: usize = 0;
        var slab_size: usize = 16;
        while (idx < 8 and slab_size < size) {
            slab_size *= 2;
            idx += 1;
        }
        return idx;
    }
};
