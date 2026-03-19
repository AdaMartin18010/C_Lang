//
// Auto-generated from: Zig\Zig_Standard_Library_Deep_Dive.md
// Line: 318
// Language: zig
// Block ID: 6823be7c
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

const std = @import("std");

fn arraylist_examples() !void {
    const allocator = std.heap.page_allocator;

    // 创建
    var list = std.ArrayList(i32).init(allocator);
    defer list.deinit();

    // 添加元素
    try list.append(10);
    try list.appendSlice(&.{ 20, 30, 40 });
    try list.insert(1, 15);  // 在索引1插入

    // 访问
    const first = list.items[0];
    const last = list.getLast();
    std.log.info("First: {}, Last: {}", .{ first, last });

    // 遍历
    for (list.items, 0..) |item, i| {
        std.log.info("[{}] = {}", .{ i, item });
    }

    // 查找
    const found = std.mem.indexOfScalar(i32, list.items, 30);
    if (found) |idx| {
        std.log.info("Found 30 at index {}", .{idx});
    }

    // 排序
    std.mem.sort(i32, list.items, {}, comptime std.sort.asc(i32));

    // 过滤 (创建新列表)
    var evens = std.ArrayList(i32).init(allocator);
    defer evens.deinit();

    for (list.items) |item| {
        if (item % 2 == 0) {
            try evens.append(item);
        }
    }

    // 映射 (转换为字符串)
    var strings = std.ArrayList([]u8).init(allocator);
    defer {
        for (strings.items) |s| {
            allocator.free(s);
        }
        strings.deinit();
    }

    for (list.items) |item| {
        const str = try std.fmt.allocPrint(allocator, "{}", .{item});
        try strings.append(str);
    }

    // 栈操作
    var stack = std.ArrayList(i32).init(allocator);
    defer stack.deinit();

    try stack.append(1);  // push
    try stack.append(2);
    const top = stack.getLast();  // peek
    _ = stack.pop();  // pop
    std.log.info("Top: {}, Remaining: {}", .{ top, stack.items.len });

    // 确保容量
    try list.ensureTotalCapacity(1000);
    list.appendAssumeCapacity(999);  // 不检查容量的添加
}
