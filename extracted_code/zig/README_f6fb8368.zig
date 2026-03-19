//
// Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\README.md
// Line: 119
// Language: zig
// Block ID: f6fb8368
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

const ExportAlloc = struct {
    var internal_gpa: ?std.heap.GeneralPurposeAllocator(.{}) = null;

    pub export fn zig_alloc_init() c_int {
        internal_gpa = std.heap.GeneralPurposeAllocator(.{}){};
        return 0;
    }

    pub export fn zig_alloc_deinit() void {
        if (internal_gpa) |*gpa| {
            _ = gpa.deinit();
            internal_gpa = null;
        }
    }

    pub export fn zig_malloc(size: usize) ?*anyopaque {
        const gpa = &(internal_gpa orelse return null);
        const ptr = gpa.allocator().alloc(u8, size) catch return null;
        return ptr.ptr;
    }

    pub export fn zig_free(ptr: ?*anyopaque, size: usize) void {
        const gpa = &(internal_gpa orelse return);
        if (ptr) |p| {
            const slice: []u8 = @as([*]u8, @ptrCast(p))[0..size];
            gpa.allocator().free(slice);
        }
    }
};
