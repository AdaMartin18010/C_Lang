//
// Auto-generated from: 08_Zig_C_Connection\05_Migration_Methodology\01_Incremental_Migration_Patterns.md
// Line: 800
// Language: zig
// Block ID: 02a9a1e7
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// C malloc/free → Zig 显式分配器

const std = @import("std");

// C 原始代码
/*
typedef struct {
    int* data;
    size_t len;
    size_t cap;
} IntVector;

IntVector* intvec_new(void) {
    IntVector* v = malloc(sizeof(IntVector));
    if (!v) return NULL;
    v->data = NULL;
    v->len = 0;
    v->cap = 0;
    return v;
}

int intvec_push(IntVector* v, int val) {
    if (v->len >= v->cap) {
        size_t new_cap = v->cap ? v->cap * 2 : 4;
        int* new_data = realloc(v->data, new_cap * sizeof(int));
        if (!new_data) return -1;
        v->data = new_data;
        v->cap = new_cap;
    }
    v->data[v->len++] = val;
    return 0;
}

void intvec_free(IntVector* v) {
    if (v) {
        free(v->data);
        free(v);
    }
}
*/

// Zig 转换代码
pub fn IntVector(comptime T: type) type {
    return struct {
        const Self = @This();

        allocator: std.mem.Allocator,
        data: []T,
        len: usize,

        pub fn init(allocator: std.mem.Allocator) Self {
            return .{
                .allocator = allocator,
                .data = &.{},
                .len = 0,
            };
        }

        pub fn deinit(self: *Self) void {
            self.allocator.free(self.data);
            self.* = undefined;
        }

        pub fn push(self: *Self, val: T) !void {
            if (self.len >= self.data.len) {
                const new_cap = if (self.data.len == 0) 4 else self.data.len * 2;
                const new_data = try self.allocator.realloc(self.data, new_cap);
                self.data = new_data;
            }
            self.data[self.len] = val;
            self.len += 1;
        }

        pub fn get(self: Self, index: usize) ?T {
            if (index >= self.len) return null;
            return self.data[index];
        }

        pub fn slice(self: Self) []const T {
            return self.data[0..self.len];
        }
    };
}

// C ABI 兼容包装
const CIntVector = extern struct {
    data: ?[*]i32,
    len: usize,
    cap: usize,
};

var global_allocator: std.mem.Allocator = std.heap.c_allocator;

export fn intvec_new() ?*CIntVector {
    const v = global_allocator.create(CIntVector) catch return null;
    v.* = .{
        .data = null,
        .len = 0,
        .cap = 0,
    };
    return v;
}

export fn intvec_push(v: ?*CIntVector, val: c_int) c_int {
    const vec = v orelse return -1;

    if (vec.len >= vec.cap) {
        const new_cap = if (vec.cap == 0) 4 else vec.cap * 2;
        const old_slice = if (vec.data) |d| d[0..vec.cap] else &[_]i32{};
        const new_slice = global_allocator.realloc(old_slice, new_cap) catch return -1;
        vec.data = new_slice.ptr;
        vec.cap = new_cap;
    }
    vec.data.?[vec.len] = val;
    vec.len += 1;
    return 0;
}

export fn intvec_free(v: ?*CIntVector) void {
    const vec = v orelse return;
    if (vec.data) |d| {
        global_allocator.free(d[0..vec.cap]);
    }
    global_allocator.destroy(vec);
}
