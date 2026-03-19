//
// Auto-generated from: Zig\Zig_C_Interoperability.md
// Line: 684
// Language: zig
// Block ID: 6aef3e97
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// C 分配器包装
pub const CAllocator = struct {
    pub fn allocator() std.mem.Allocator {
        return .{
            .ptr = undefined,
            .vtable = &vtable,
        };
    }

    const vtable = std.mem.Allocator.VTable{
        .alloc = alloc,
        .resize = resize,
        .free = free,
    };

    fn alloc(_: *anyopaque, len: usize, ptr_align: u8, _: usize) ?[*]u8 {
        const ptr = c.malloc(len);
        if (ptr_align > 1) {
            // 确保对齐
            const alignment = @as(usize, 1) << @intCast(ptr_align);
            _ = alignment;
        }
        return @ptrCast(ptr);
    }

    fn resize(_: *anyopaque, buf: []u8, _: u8, new_len: usize, _: usize) bool {
        if (new_len <= buf.len) return true;
        return false;
    }

    fn free(_: *anyopaque, buf: []u8, _: u8, _: usize) void {
        c.free(buf.ptr);
    }
};

// 使用示例
pub fn use_c_allocator() void {
    var list = std.ArrayList(u8).init(CAllocator.allocator());
    defer list.deinit();

    list.appendSlice("hello") catch return;
}
