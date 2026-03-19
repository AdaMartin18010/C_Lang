//
// Auto-generated from: Zig\Zig_Standard_Library_Deep_Dive.md
// Line: 468
// Language: zig
// Block ID: 05fd6027
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

fn other_containers() !void {
    const allocator = std.heap.page_allocator;

    // BufMap - 字符串到字符串的映射
    var headers = std.BufMap.init(allocator);
    defer headers.deinit();

    try headers.put("Content-Type", "application/json");
    try headers.put("Authorization", "Bearer token123");

    if (headers.get("Content-Type")) |ct| {
        std.log.info("Content-Type: {s}", .{ct});
    }

    // PriorityQueue (二叉堆)
    const PQ = std.PriorityQueue(i32, void, struct {
        fn lessThan(_: void, a: i32, b: i32) std.math.Order {
            return std.math.order(a, b);  // 最小堆
        }
    }.lessThan);

    var pq = PQ.init(allocator, {});
    defer pq.deinit();

    try pq.add(5);
    try pq.add(2);
    try pq.add(8);
    try pq.add(1);

    while (pq.removeOrNull()) |min| {
        std.log.info("Min: {}", .{min});  // 1, 2, 5, 8
    }

    // LinkedList
    var list = std.SinglyLinkedList(i32){};

    var node1 = std.SinglyLinkedList(i32).Node{ .data = 1 };
    var node2 = std.SinglyLinkedList(i32).Node{ .data = 2 };
    var node3 = std.SinglyLinkedList(i32).Node{ .data = 3 };

    list.prepend(&node3);
    list.prepend(&node2);
    list.prepend(&node1);

    var it = list.first;
    while (it) |node| : (it = node.next) {
        std.log.info("Node: {}", .{node.data});
    }

    // SegmentedList - 避免重新分配
    var seg_list = std.SegmentedList(i32, 8).init(allocator);
    defer seg_list.deinit();

    for (0..100) |i| {
        try seg_list.append(@intCast(i));
    }

    // BitSet
    var bit_set = std.DynamicBitSet.initEmpty(allocator, 1000);
    defer bit_set.deinit();

    bit_set.set(100);
    bit_set.set(200);
    bit_set.toggle(100);

    std.log.info("100: {}, 200: {}", .{ bit_set.isSet(100), bit_set.isSet(200) });

    // 遍历所有设置的位
    var bit_it = bit_set.iterator(.{});
    while (bit_it.next()) |bit| {
        std.log.info("Bit {} is set", .{bit});
    }
}
