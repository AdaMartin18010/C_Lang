//
// Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\01_C11_vs_Zig_Memory_Model.md
// Line: 1453
// Language: zig
// Block ID: 4d533480
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig 无锁队列 (Michael-Scott 算法简化版)
const std = @import("std");
const Atomic = std.atomic.Value;

pub fn LockFreeQueue(comptime T: type) type {
    return struct {
        const Self = @This();

        const Node = struct {
            data: ?T,
            next: Atomic(?*Node),
        };

        head: Atomic(*Node),
        tail: Atomic(*Node),
        allocator: std.mem.Allocator,

        pub fn init(allocator: std.mem.Allocator) !Self {
            const dummy = try allocator.create(Node);
            dummy.* = .{
                .data = null,
                .next = Atomic(?*Node).init(null),
            };
            return .{
                .head = Atomic(*Node).init(dummy),
                .tail = Atomic(*Node).init(dummy),
                .allocator = allocator,
            };
        }

        pub fn enqueue(self: *Self, data: T) !void {
            const node = try self.allocator.create(Node);
            node.* = .{
                .data = data,
                .next = Atomic(?*Node).init(null),
            };

            while (true) {
                const tail = self.tail.load(.Acquire);
                const next = tail.next.load(.Acquire);

                // 检查 tail 是否仍然有效
                if (tail == self.tail.load(.Acquire)) {
                    if (next == null) {
                        // 尝试链接新节点
                        const result = tail.next.tryCmpxchg(
                            null,
                            node,
                            .Release,
                            .Monotonic
                        );
                        if (result == null) {
                            // 成功，尝试移动 tail
                            _ = self.tail.tryCmpxchg(tail, node, .Release, .Monotonic);
                            return;
                        }
                    } else {
                        // 帮助移动 tail
                        _ = self.tail.tryCmpxchg(tail, next.?, .Release, .Monotonic);
                    }
                }
            }
        }

        pub fn dequeue(self: *Self) ?T {
            while (true) {
                const head = self.head.load(.Acquire);
                const tail = self.tail.load(.Acquire);
                const next = head.next.load(.Acquire);

                if (head == self.head.load(.Acquire)) {
                    if (head == tail) {
                        if (next == null) {
                            return null;  // 队列为空
                        }
                        // 帮助移动 tail
                        _ = self.tail.tryCmpxchg(tail, next.?, .Release, .Monotonic);
                    } else {
                        const data = next.?.data.?;
                        const result = self.head.tryCmpxchg(
                            head,
                            next.?,
                            .Release,
                            .Monotonic
                        );
                        if (result == null) {
                            // 成功
                            self.allocator.destroy(head);
                            return data;
                        }
                    }
                }
            }
        }
    };
}
