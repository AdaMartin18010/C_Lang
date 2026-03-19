//
// Auto-generated from: Zig\Zig_Concurrency_Async.md
// Line: 503
// Language: zig
// Block ID: 64088fd5
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Michael-Scott 无锁队列
const std = @import("std");
const Atomic = std.atomic.Atomic;

pub fn LockFreeQueue(comptime T: type) type {
    return struct {
        const Node = struct {
            data: T,
            next: Atomic(?*Node),
        };

        head: Atomic(*Node),
        tail: Atomic(*Node),
        allocator: std.mem.Allocator,

        pub fn init(allocator: std.mem.Allocator) !@This() {
            const dummy = try allocator.create(Node);
            dummy.* = .{
                .data = undefined,
                .next = Atomic(?*Node).init(null),
            };

            return .{
                .head = Atomic(*Node).init(dummy),
                .tail = Atomic(*Node).init(dummy),
                .allocator = allocator,
            };
        }

        pub fn deinit(self: *@This()) void {
            // 清理所有节点
            var current = self.head.load(.Acquire);
            while (current) |node| {
                const next = node.next.load(.Acquire);
                self.allocator.destroy(node);
                current = next;
            }
        }

        pub fn enqueue(self: *@This(), data: T) !void {
            const new_node = try self.allocator.create(Node);
            new_node.* = .{
                .data = data,
                .next = Atomic(?*Node).init(null),
            };

            var tail = self.tail.load(.Acquire);
            while (true) {
                const next = tail.next.load(.Acquire);

                if (next == null) {
                    // 尝试链接新节点
                    if (tail.next.compareAndSwap(null, new_node, .Release, .Acquire) == null) {
                        // 成功，尝试更新 tail
                        _ = self.tail.compareAndSwap(tail, new_node, .Release, .Acquire);
                        return;
                    }
                } else {
                    // Tail 落后，尝试推进
                    tail = self.tail.load(.Acquire);
                }
            }
        }

        pub fn dequeue(self: *@This()) ?T {
            var head = self.head.load(.Acquire);

            while (true) {
                const tail = self.tail.load(.Acquire);
                const next = head.next.load(.Acquire);

                if (head == tail) {
                    if (next == null) return null;  // 空队列
                    // Tail 落后
                    _ = self.tail.compareAndSwap(tail, next.?, .Release, .Acquire);
                } else {
                    const data = next.?.data;
                    if (self.head.compareAndSwap(head, next.?, .Release, .Acquire) == null) {
                        self.allocator.destroy(head);
                        return data;
                    }
                }
                head = self.head.load(.Acquire);
            }
        }
    };
}

// 使用示例
pub fn main() !void {
    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    defer _ = gpa.deinit();
    const allocator = gpa.allocator();

    var queue = try LockFreeQueue(i32).init(allocator);
    defer queue.deinit();

    // 多线程测试
    const Producer = struct {
        fn run(q: *LockFreeQueue(i32)) void {
            var i: i32 = 0;
            while (i < 1000) : (i += 1) {
                q.enqueue(i) catch continue;
            }
        }
    };

    const Consumer = struct {
        fn run(q: *LockFreeQueue(i32)) void {
            var count: usize = 0;
            while (count < 1000) {
                if (q.dequeue()) |_| {
                    count += 1;
                }
            }
        }
    };

    const p1 = try std.Thread.spawn(.{}, Producer.run, .{&queue});
    const p2 = try std.Thread.spawn(.{}, Producer.run, .{&queue});
    const c1 = try std.Thread.spawn(.{}, Consumer.run, .{&queue});
    const c2 = try std.Thread.spawn(.{}, Consumer.run, .{&queue});

    p1.join();
    p2.join();
    c1.join();
    c2.join();

    std.log.info("Queue test completed", .{});
}
