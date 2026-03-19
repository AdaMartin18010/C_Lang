//
// Auto-generated from: 08_Zig_C_Connection\06_C23_Zig_Parallel\03_Typeof_vs_TypeOf.md
// Line: 2465
// Language: zig
// Block ID: a032c411
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig: 泛型链表实现

const std = @import("std");

fn LinkedList(comptime T: type) type {
    return struct {
        const Node = struct {
            data: T,
            next: ?*Node,
        };

        head: ?*Node,
        tail: ?*Node,
        size: usize,
        allocator: std.mem.Allocator,

        const Self = @This();

        fn init(allocator: std.mem.Allocator) Self {
            return .{
                .head = null,
                .tail = null,
                .size = 0,
                .allocator = allocator,
            };
        }

        fn pushFront(self: *Self, value: T) !void {
            const node = try self.allocator.create(Node);
            node.* = .{
                .data = value,
                .next = self.head,
            };
            self.head = node;
            if (self.tail == null) {
                self.tail = node;
            }
            self.size += 1;
        }

        fn popFront(self: *Self) ?T {
            const node = self.head orelse return null;
            const value = node.data;
            self.head = node.next;
            if (self.head == null) {
                self.tail = null;
            }
            self.allocator.destroy(node);
            self.size -= 1;
            return value;
        }

        fn deinit(self: *Self) void {
            while (self.popFront() != null) {}
        }
    };
}

// 使用
fn demoZigList() !void {
    const allocator = std.heap.page_allocator;

    var list = LinkedList(i32).init(allocator);
    defer list.deinit();

    try list.pushFront(10);
    try list.pushFront(20);

    while (list.popFront()) |val| {
        std.debug.print("{}\n", .{val});
    }
}
