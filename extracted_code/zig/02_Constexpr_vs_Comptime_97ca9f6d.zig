//
// Auto-generated from: 08_Zig_C_Connection\06_C23_Zig_Parallel\02_Constexpr_vs_Comptime.md
// Line: 897
// Language: zig
// Block ID: 97ca9f6d
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig comptime 泛型编程
const std = @import("std");

// ========== 基础泛型函数 ==========

// 类型参数函数
fn max(comptime T: type, a: T, b: T) T {
    return if (a > b) a else b;
}

// 多个类型参数
fn pair(comptime T: type, comptime U: type, first: T, second: U) struct { T, U } {
    return .{ first, second };
}

// ========== 泛型数据结构 ==========

// 完全泛型动态数组
fn ArrayList(comptime T: type) type {
    return struct {
        items: []T,
        capacity: usize,
        allocator: std.mem.Allocator,

        const Self = @This();

        fn init(allocator: std.mem.Allocator) Self {
            return .{
                .items = &[_]T{},
                .capacity = 0,
                .allocator = allocator,
            };
        }

        fn deinit(self: *Self) void {
            self.allocator.free(self.items);
        }

        fn append(self: *Self, item: T) !void {
            if (self.items.len >= self.capacity) {
                try self.grow();
            }
            self.items[self.items.len] = item;
            self.items.len += 1;
        }

        fn grow(self: *Self) !void {
            const new_capacity = if (self.capacity == 0) 8 else self.capacity * 2;
            const new_items = try self.allocator.realloc(self.items, new_capacity);
            self.items = new_items;
            self.capacity = new_capacity;
        }
    };
}

// 实例化
const IntList = ArrayList(i32);
const StringList = ArrayList([]const u8);

// ========== 类型约束（编译时接口） ==========

// 检查类型是否可比较
fn isComparable(comptime T: type) bool {
    return @hasDecl(T, "compare");
}

// 泛型排序（带约束检查）
fn sort(comptime T: type, arr: []T) void {
    comptime {
        // 检查 T 是否支持 < 操作
        const info = @typeInfo(T);
        if (info != .Int and info != .Float) {
            @compileError("Type must be numeric for default sort");
        }
    }
    std.sort.heap(T, arr, {}, std.sort.asc(T));
}

// 自定义比较器接口
fn sortWith(comptime T: type, arr: []T, comptime compare: fn (T, T) bool) void {
    // 使用自定义比较器排序...
    _ = arr;
    _ = compare;
}

// ========== 高级类型构造 ==========

// 可选类型构造
fn Optional(comptime T: type) type {
    return union(enum) {
        none,
        some: T,

        pub fn isSome(self: @This()) bool {
            return self == .some;
        }

        pub fn unwrap(self: @This()) T {
            return switch (self) {
                .some => |v| v,
                .none => @panic("Unwrap on none"),
            };
        }
    };
}

// 结果类型构造
fn Result(comptime T: type, comptime E: type) type {
    return union(enum) {
        ok: T,
        err: E,

        pub fn isOk(self: @This()) bool {
            return self == .ok;
        }
    };
}

// ========== 编译时多态 ==========

// 根据类型特性选择实现
fn optimizedCopy(comptime T: type, dest: []T, src: []const T) void {
    comptime {
        if (@sizeOf(T) == 1) {
            // 字节级优化：使用 memcmp
            @memcpy(dest[0..src.len], src);
        } else if (@alignOf(T) >= @sizeOf(usize)) {
            // 对齐良好：使用 SIMD
            for (dest, src) |*d, s| {
                d.* = s;
            }
        } else {
            // 默认实现
            for (dest, src) |*d, s| {
                d.* = s;
            }
        }
    }
}

// ========== 泛型特化 ==========

// 基础版本
trait Container(T: type) = struct {
    fn get(self: T, idx: usize) T.Item;
    fn len(self: T) usize;
};

// 为特定类型提供特化
fn fastSum(comptime T: type, arr: []const T) T {
    comptime {
        if (T == i32) {
            // i32 特化：使用 SIMD
            return simdSumI32(arr);
        } else if (T == f32) {
            // f32 特化：使用 FMA
            return fmaSumF32(arr);
        } else {
            // 通用版本
            return genericSum(T, arr);
        }
    }
}

fn simdSumI32(arr: []const i32) i32 {
    _ = arr;
    return 0;  // 简化示例
}

fn fmaSumF32(arr: []const f32) f32 {
    _ = arr;
    return 0.0;  // 简化示例
}

fn genericSum(comptime T: type, arr: []const T) T {
    var sum: T = 0;
    for (arr) |item| {
        sum += item;
    }
    return sum;
}
