//
// Auto-generated from: Zig\Zig_Complete_Guide.md
// Line: 595
// Language: zig
// Block ID: f5eb4eac
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

const std = @import("std");

// Zig 的核心理念：显式分配器
// 不像 C 的 malloc/free，Zig 要求传递分配器

pub fn main() !void {
    // 获取通用分配器
    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    defer _ = gpa.deinit();
    const allocator = gpa.allocator();

    // 动态分配内存
    const buf = try allocator.alloc(u8, 100);
    defer allocator.free(buf);  // 确保释放

    // 使用 buf
    @memset(buf, 0);

    // 重新分配
    const larger_buf = try allocator.realloc(buf, 200);
    // 注意：如果 realloc 失败，原来的 buf 仍然有效
    // 所以通常需要重新赋值

    // 分配单个对象
    const ptr = try allocator.create(MyStruct);
    defer allocator.destroy(ptr);
    ptr.* = MyStruct{ .x = 1, .y = 2 };

    // 使用 ArrayList (动态数组)
    var list = std.ArrayList(u32).init(allocator);
    defer list.deinit();

    try list.append(1);
    try list.append(2);
    try list.append(3);

    std.debug.print("ArrayList: ", .{});
    for (list.items) |item| {
        std.debug.print("{} ", .{item});
    }
    std.debug.print("\n", .{});

    // 使用 HashMap
    var map = std.StringHashMap(i32).init(allocator);
    defer map.deinit();

    try map.put("one", 1);
    try map.put("two", 2);

    if (map.get("one")) |value| {
        std.debug.print("one = {}\n", .{value});
    }
}

const MyStruct = struct {
    x: i32,
    y: i32,
};

// 自定义分配器： arena 分配器
fn use_arena_allocator() !void {
    var arena = std.heap.ArenaAllocator.init(std.heap.page_allocator);
    defer arena.deinit();  // 一次性释放所有内存
    const allocator = arena.allocator();

    // 分配大量临时内存
    _ = try allocator.alloc(u8, 100);
    _ = try allocator.alloc(u8, 200);
    _ = try allocator.alloc(u8, 300);
    // 不需要单独 free，arena.deinit() 会释放所有
}

// 固定缓冲区分配器 (无堆分配)
fn use_fixed_buffer_allocator() void {
    var buffer: [1024]u8 = undefined;
    var fba = std.heap.FixedBufferAllocator.init(&buffer);
    const allocator = fba.allocator();

    const data = allocator.alloc(u8, 100) catch {
        std.debug.print("Out of memory!\n", .{});
        return;
    };
    _ = data;
}
