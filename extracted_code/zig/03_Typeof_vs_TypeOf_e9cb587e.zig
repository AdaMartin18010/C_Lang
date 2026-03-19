//
// Auto-generated from: 08_Zig_C_Connection\06_C23_Zig_Parallel\03_Typeof_vs_TypeOf.md
// Line: 1320
// Language: zig
// Block ID: e9cb587e
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig: 使用 comptime 和 @TypeOf 实现泛型栈

const std = @import("std");

fn Stack(comptime T: type) type {
    return struct {
        data: std.ArrayList(T),

        const Self = @This();

        fn init(allocator: std.mem.Allocator) Self {
            return .{
                .data = std.ArrayList(T).init(allocator),
            };
        }

        fn deinit(self: *Self) void {
            self.data.deinit();
        }

        fn push(self: *Self, value: T) !void {
            try self.data.append(value);
        }

        fn pop(self: *Self) ?T {
            if (self.data.items.len == 0) return null;
            return self.data.pop();
        }

        fn peek(self: Self) ?T {
            if (self.data.items.len == 0) return null;
            return self.data.items[self.data.items.len - 1];
        }

        fn isEmpty(self: Self) bool {
            return self.data.items.len == 0;
        }

        fn size(self: Self) usize {
            return self.data.items.len;
        }
    };
}

// 使用 @TypeOf 的便利函数
fn makeStack(value: anytype) Stack(@TypeOf(value)) {
    return Stack(@TypeOf(value));
}

fn demoZigStack() !void {
    const allocator = std.heap.page_allocator;

    // 显式类型
    var int_stack = Stack(i32).init(allocator);
    defer int_stack.deinit();

    try int_stack.push(10);
    try int_stack.push(20);

    if (int_stack.pop()) |val| {
        std.debug.print("Popped: {}\n", .{val});
    }

    // 类型推断
    var inferred_stack = makeStack(3.14);
    defer inferred_stack.deinit();

    try inferred_stack.push(2.71);
}
