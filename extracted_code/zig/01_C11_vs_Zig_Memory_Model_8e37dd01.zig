//
// Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\01_C11_vs_Zig_Memory_Model.md
// Line: 1788
// Language: zig
// Block ID: 8e37dd01
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig 引用计数智能指针
const std = @import("std");
const Atomic = std.atomic.Value;

pub fn Rc(comptime T: type) type {
    return struct {
        const Self = @This();

        const Inner = struct {
            count: Atomic(usize),
            value: T,
        };

        ptr: *Inner,

        pub fn create(allocator: std.mem.Allocator, value: T) !Self {
            const ptr = try allocator.create(Inner);
            ptr.* = .{
                .count = Atomic(usize).init(1),
                .value = value,
            };
            return .{ .ptr = ptr };
        }

        pub fn clone(self: Self) Self {
            // Relaxed: 仅计数，不需要顺序保证
            _ = self.ptr.count.fetchAdd(1, .Monotonic);
            return .{ .ptr = self.ptr };
        }

        pub fn drop(self: *Self, allocator: std.mem.Allocator) void {
            // Release: 确保之前的写入在可能销毁前可见
            const old_count = self.ptr.count.fetchSub(1, .Release);

            if (old_count == 1) {
                // 最后一个引用
                // Acquire: 确保看到 count==0 后，能看到所有 clone 的操作
                std.atomic.fence(.Acquire);

                // 销毁值
                self.ptr.value.deinit();

                // 释放内存
                allocator.destroy(self.ptr);
            }

            self.ptr = undefined;
        }

        pub fn get(self: Self) *T {
            return &self.ptr.value;
        }

        pub fn getConst(self: Self) *const T {
            return &self.ptr.value;
        }
    };
}

// 使用示例
const MyData = struct {
    value: i32,

    pub fn deinit(self: *MyData) void {
        std.debug.print("Destroying data with value {d}\n", .{self.value});
    }
};

fn example() !void {
    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    defer _ = gpa.deinit();
    const allocator = gpa.allocator();

    var rc1 = try Rc(MyData).create(allocator, .{ .value = 42 });
    rc1.get().value = 100;

    var rc2 = rc1.clone();  // 引用计数 = 2

    rc1.drop(allocator);  // 引用计数 = 1，不销毁
    rc2.drop(allocator);  // 引用计数 = 0，销毁
}
