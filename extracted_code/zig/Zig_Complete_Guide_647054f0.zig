//
// Auto-generated from: Zig\Zig_Complete_Guide.md
// Line: 1427
// Language: zig
// Block ID: 647054f0
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

const std = @import("std");

// 泛型栈
fn Stack(comptime T: type) type {
    return struct {
        const Self = @This();

        items: []T,
        capacity: usize,
        len: usize,
        allocator: std.mem.Allocator,

        pub fn init(allocator: std.mem.Allocator) !Self {
            const initial_capacity = 16;
            const items = try allocator.alloc(T, initial_capacity);
            return Self{
                .items = items,
                .capacity = initial_capacity,
                .len = 0,
                .allocator = allocator,
            };
        }

        pub fn deinit(self: *Self) void {
            self.allocator.free(self.items);
        }

        pub fn push(self: *Self, item: T) !void {
            if (self.len >= self.capacity) {
                const new_capacity = self.capacity * 2;
                self.items = try self.allocator.realloc(self.items, new_capacity);
                self.capacity = new_capacity;
            }
            self.items[self.len] = item;
            self.len += 1;
        }

        pub fn pop(self: *Self) ?T {
            if (self.len == 0) return null;
            self.len -= 1;
            return self.items[self.len];
        }

        pub fn peek(self: Self) ?T {
            if (self.len == 0) return null;
            return self.items[self.len - 1];
        }
    };
}

// 泛型排序
fn quickSort(comptime T: type, items: []T, comptime cmp: fn (T, T) bool) void {
    if (items.len <= 1) return;

    const pivot = items[items.len / 2];
    var i: usize = 0;
    var j: usize = items.len - 1;

    while (i <= j) {
        while (cmp(items[i], pivot)) i += 1;
        while (cmp(pivot, items[j])) j -= 1;
        if (i <= j) {
            const tmp = items[i];
            items[i] = items[j];
            items[j] = tmp;
            i += 1;
            if (j > 0) j -= 1;
        }
    }

    if (j > 0) quickSort(T, items[0..j + 1], cmp);
    quickSort(T, items[i..], cmp);
}

fn lessThan(a: i32, b: i32) bool {
    return a < b;
}

pub fn main() !void {
    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    defer _ = gpa.deinit();
    const allocator = gpa.allocator();

    // 使用泛型栈
    var int_stack = try Stack(i32).init(allocator);
    defer int_stack.deinit();

    try int_stack.push(10);
    try int_stack.push(20);
    try int_stack.push(30);

    while (int_stack.pop()) |val| {
        std.debug.print("Popped: {}\n", .{val});
    }

    // 泛型排序
    var arr = [_]i32{ 5, 2, 8, 1, 9, 3 };
    quickSort(i32, &arr, lessThan);
    std.debug.print("Sorted: {any}\n", .{arr});

    // 使用标准库的 sort
    std.mem.sort(i32, &arr, {}, std.sort.asc(i32));
    std.debug.print("Sorted (std): {any}\n", .{arr});
}
