//
// Auto-generated from: Zig\Zig_Performance_Optimization.md
// Line: 332
// Language: zig
// Block ID: d302d04d
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

const std = @import("std");

// 使用合适的分配器
pub fn allocationStrategies() !void {
    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    defer _ = gpa.deinit();
    const allocator = gpa.allocator();

    // 1. 小对象使用固定缓冲区分配器
    var buffer: [4096]u8 = undefined;
    var fba = std.heap.FixedBufferAllocator.init(&buffer);
    const small_allocator = fba.allocator();

    const small_data = try small_allocator.alloc(u8, 100);
    defer small_allocator.free(small_data);

    // 2. Arena 分配器 - 批量释放
    var arena = std.heap.ArenaAllocator.init(allocator);
    defer arena.deinit();
    const arena_allocator = arena.allocator();

    // 多次分配，只需一次释放
    _ = try arena_allocator.alloc(u8, 100);
    _ = try arena_allocator.alloc(u8, 200);
    _ = try arena_allocator.alloc(u8, 300);

    // 3. 对象池 - 重用对象
    const Pool = std.heap.MemoryPool(MyStruct);
    var pool = try Pool.initPreheated(allocator, 100);
    defer pool.deinit();

    const obj = try pool.create();
    defer pool.destroy(obj);
}

const MyStruct = struct {
    data: [256]u8,
    len: usize,
};
