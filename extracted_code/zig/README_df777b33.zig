//
// Auto-generated from: Zig\README.md
// Line: 1050
// Language: zig
// Block ID: df777b33
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 谁分配谁释放原则

// Zig 分配的内存，Zig 释放
export fn zig_allocate_buffer(size: usize) ?[*]u8 {
    const slice = std.heap.c_allocator.alloc(u8, size) catch return null;
    return slice.ptr;
}

export fn zig_free_buffer(ptr: ?[*]u8, size: usize) void {
    if (ptr) |p| {
        const slice = p[0..size];
        std.heap.c_allocator.free(slice);
    }
}

// C 分配的内存，C 释放（通过回调）
export fn zig_process_data(
    data: [*]const u8,
    len: usize,
    callback: ?*const fn (?*anyopaque) void,
    context: ?*anyopaque,
) void {
    // 处理数据...
    if (callback) |cb| {
        cb(context);
    }
}
