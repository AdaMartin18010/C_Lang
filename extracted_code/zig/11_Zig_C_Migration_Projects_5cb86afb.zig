//
// Auto-generated from: 12_Practice_Exercises\11_Zig_C_Migration_Projects.md
// Line: 79
// Language: zig
// Block ID: 5cb86afb
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// list.zig
const std = @import("std");

pub fn List(comptime T: type) type {
    return struct {
        const Self = @This();

        pub const Node = struct {
            data: T,
            next: ?*Node,
        };

        head: ?*Node,
        tail: ?*Node,
        len: usize,
        allocator: std.mem.Allocator,

        pub fn init(allocator: std.mem.Allocator) Self {
            return .{
                .head = null,
                .tail = null,
                .len = 0,
                .allocator = allocator,
            };
        }

        pub fn deinit(self: *Self) void {
            var current = self.head;
            while (current) |node| {
                current = node.next;
                self.allocator.destroy(node);
            }
            self.* = undefined;
        }

        pub fn append(self: *Self, value: T) !void {
            const node = try self.allocator.create(Node);
            node.* = .{ .data = value, .next = null };

            if (self.tail) |tail| {
                tail.next = node;
            } else {
                self.head = node;
            }
            self.tail = node;
            self.len += 1;
        }

        pub fn remove(self: *Self, value: T) bool {
            var current = &self.head;
            while (current.*) |node| {
                if (node.data == value) {
                    current.* = node.next;
                    if (self.tail == node) {
                        self.tail = if (current.* == node) null else current.*;
                    }
                    self.allocator.destroy(node);
                    self.len -= 1;
                    return true;
                }
                current = &node.next;
            }
            return false;
        }

        pub fn toArray(self: Self, allocator: std.mem.Allocator) ![]T {
            var arr = try allocator.alloc(T, self.len);
            var i: usize = 0;
            var current = self.head;
            while (current) |node| : (current = node.next) {
                arr[i] = node.data;
                i += 1;
            }
            return arr;
        }
    };
}
