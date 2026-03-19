//
// Auto-generated from: Zig\Zig_Complete_Guide.md
// Line: 1286
// Language: zig
// Block ID: 2128c3ef
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

const std = @import("std");

pub fn main() !void {
    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    defer _ = gpa.deinit();
    const allocator = gpa.allocator();

    // ArrayList - 动态数组
    var list = std.ArrayList(i32).init(allocator);
    defer list.deinit();

    try list.append(1);
    try list.append(2);
    try list.insert(1, 99);

    // 遍历
    for (list.items, 0..) |item, i| {
        std.debug.print("[{}] = {}\n", .{ i, item });
    }

    // HashMap
    var map = std.StringHashMap(f64).init(allocator);
    defer map.deinit();

    try map.put("pi", 3.14159);
    try map.put("e", 2.71828);

    var it = map.iterator();
    while (it.next()) |entry| {
        std.debug.print("{s} = {}\n", .{ entry.key_ptr.*, entry.value_ptr.* });
    }

    // 自动哈希 map (BufMap)
    var buf_map = std.BufMap.init(allocator);
    defer buf_map.deinit();

    try buf_map.put("key1", "value1");
    if (buf_map.get("key1")) |val| {
        std.debug.print("Found: {s}\n", .{val});
    }

    // 优先队列 (PriorityQueue)
    const PQ = std.PriorityQueue(i32, void, struct {
        fn lessThan(_: void, a: i32, b: i32) std.math.Order {
            return std.math.order(a, b);
        }
    }.lessThan);

    var pq = PQ.init(allocator, {});
    defer pq.deinit();

    try pq.add(5);
    try pq.add(1);
    try pq.add(3);

    while (pq.removeOrNull()) |min| {
        std.debug.print("Min: {}\n", .{min});
    }

    // 位集合
    var bit_set = std.DynamicBitSet.initEmpty(allocator, 100);
    defer bit_set.deinit();

    bit_set.set(10);
    bit_set.set(20);
    bit_set.toggle(10);
    std.debug.print("Bit 10: {}, Bit 20: {}\n", .{ bit_set.isSet(10), bit_set.isSet(20) });
}
