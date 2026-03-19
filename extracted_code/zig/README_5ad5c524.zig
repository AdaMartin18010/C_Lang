//
// Auto-generated from: Zig\README.md
// Line: 1278
// Language: zig
// Block ID: 5ad5c524
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

fn LinkedList(comptime T: type) type {
    return struct {
        pub const Node = struct {
            data: T,
            next: ?*Node,
        };

        first: ?*Node,
        allocator: std.mem.Allocator,

        const Self = @This();

        pub fn init(allocator: std.mem.Allocator) Self {
            return .{
                .first = null,
                .allocator = allocator,
            };
        }

        pub fn push(self: *Self, data: T) !void {
            const node = try self.allocator.create(Node);
            node.* = .{
                .data = data,
                .next = self.first,
            };
            self.first = node;
        }

        pub fn pop(self: *Self) ?T {
            const node = self.first orelse return null;
            defer self.allocator.destroy(node);
            self.first = node.next;
            return node.data;
        }
    };
}

// 使用泛型链表
const IntList = LinkedList(i32);
const StringList = LinkedList([]const u8);
