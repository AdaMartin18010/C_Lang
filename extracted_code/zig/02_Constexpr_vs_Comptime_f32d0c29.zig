//
// Auto-generated from: 08_Zig_C_Connection\06_C23_Zig_Parallel\02_Constexpr_vs_Comptime.md
// Line: 2296
// Language: zig
// Block ID: f32d0c29
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig: 类型安全容器
const std = @import("std");

// 泛型动态数组
fn ArrayList(comptime T: type) type {
    return struct {
        items: []T,
        capacity: usize,
        allocator: std.mem.Allocator,

        const Self = @This();
        const Error = error{OutOfMemory};

        pub fn init(allocator: std.mem.Allocator) Self {
            return .{
                .items = &[_]T{},
                .capacity = 0,
                .allocator = allocator,
            };
        }

        pub fn deinit(self: *Self) void {
            self.allocator.free(self.items);
            self.* = undefined;
        }

        pub fn append(self: *Self, item: T) Error!void {
            if (self.items.len >= self.capacity) {
                try self.grow();
            }
            self.items[self.items.len] = item;
            self.items.len += 1;
        }

        pub fn get(self: Self, idx: usize) T {
            return self.items[idx];  // 边界检查在安全模式下
        }

        pub fn set(self: *Self, idx: usize, item: T) void {
            self.items[idx] = item;
        }

        fn grow(self: *Self) Error!void {
            const new_capacity = if (self.capacity == 0) 8 else self.capacity * 2;
            const new_items = try self.allocator.realloc(self.items, new_capacity);
            self.items = new_items;
            self.capacity = new_capacity;
        }

        // 编译时类型特定优化
        pub fn sum(self: Self) T {
            comptime {
                // 检查 T 是否支持加法
                if (!@hasDecl(T, "__add__") and
                    @typeInfo(T) != .Int and
                    @typeInfo(T) != .Float) {
                    @compileError("Type must support addition");
                }
            }
            var total: T = 0;
            for (self.items) |item| {
                total += item;
            }
            return total;
        }
    };
}

// 实例化（单态化）
const IntList = ArrayList(i32);
const FloatList = ArrayList(f64);

// 使用
fn example() !void {
    var list = IntList.init(std.heap.page_allocator);
    defer list.deinit();

    try list.append(42);
    try list.append(100);

    const x = list.get(0);  // i32
    const total = list.sum();  // 编译时验证 i32 支持加法
}

// 高级：编译时类型检查
fn requireNumeric(comptime T: type) void {
    comptime {
        const info = @typeInfo(T);
        if (info != .Int and info != .Float) {
            @compileError("Type must be numeric");
        }
    }
}

fn NumericArrayList(comptime T: type) type {
    comptime requireNumeric(T);
    return ArrayList(T);
}

// const StringList = NumericArrayList([]const u8);  // 编译错误
