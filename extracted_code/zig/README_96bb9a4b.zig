//
// Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\README.md
// Line: 78
// Language: zig
// Block ID: 96bb9a4b
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

const c = @cImport({
    @cInclude("stdlib.h");
    @cInclude("string.h");
});

/// 包装 C 的 malloc/free，确保与 Zig 错误处理兼容
pub const CAllocator = struct {
    pub fn alloc(len: usize) ?[*]u8 {
        return @ptrCast(c.malloc(len));
    }

    pub fn free(ptr: ?[*]u8) void {
        c.free(@ptrCast(ptr));
    }

    pub fn realloc(ptr: ?[*]u8, new_len: usize) ?[*]u8 {
        return @ptrCast(c.realloc(@ptrCast(ptr), new_len));
    }
};

// 使用示例：与返回 malloc 内存的 C 库交互
pub fn processCData() !void {
    // 假设 C 函数: char* get_data(size_t* len);
    var len: usize = 0;
    const c_data = c.get_data(&len);
    if (c_data == null) return error.CAllocationFailed;
    defer c.free(c_data);  // 确保释放 C 分配的内存

    // 安全地转换为 Zig slice
    const zig_slice: []const u8 = @as([*]const u8, @ptrCast(c_data))[0..len];

    // 处理数据...
    std.log.info("收到 {d} 字节数据", .{zig_slice.len});
}
