//
// Auto-generated from: Zig\Zig_Machine_Learning.md
// Line: 47
// Language: zig
// Block ID: dad1ccab
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

const std = @import("std");

pub const Tensor = struct {
    allocator: std.mem.Allocator,
    data: []f32,
    shape: []const usize,
    strides: []usize,

    pub fn init(allocator: std.mem.Allocator, shape: []const usize) !Tensor {
        var size: usize = 1;
        for (shape) |dim| size *= dim;

        const data = try allocator.alloc(f32, size);
        @memset(data, 0);

        const strides = try allocator.alloc(usize, shape.len);
        var stride: usize = 1;
        var i: usize = shape.len;
        while (i > 0) : (i -= 1) {
            strides[i - 1] = stride;
            stride *= shape[i - 1];
        }

        return .{
            .allocator = allocator,
            .data = data,
            .shape = try allocator.dupe(usize, shape),
            .strides = strides,
        };
    }

    pub fn initRandom(allocator: std.mem.Allocator, shape: []const usize, min: f32, max: f32) !Tensor {
        var tensor = try init(allocator, shape);

        var prng = std.rand.DefaultPrng.init(@intCast(std.time.milliTimestamp()));
        const random = prng.random();

        for (tensor.data) |*val| {
            val.* = min + random.float(f32) * (max - min);
        }

        return tensor;
    }

    pub fn deinit(self: Tensor) void {
        self.allocator.free(self.data);
        self.allocator.free(self.shape);
        self.allocator.free(self.strides);
    }

    pub fn get(self: Tensor, indices: []const usize) f32 {
        var idx: usize = 0;
        for (indices, self.strides) |i, s| {
            idx += i * s;
        }
        return self.data[idx];
    }

    pub fn set(self: Tensor, indices: []const usize, value: f32) void {
        var idx: usize = 0;
        for (indices, self.strides) |i, s| {
            idx += i * s;
        }
        self.data[idx] = value;
    }

    pub fn shapeEqual(self: Tensor, other: Tensor) bool {
        if (self.shape.len != other.shape.len) return false;
        for (self.shape, other.shape) |a, b| {
            if (a != b) return false;
        }
        return true;
    }

    pub fn format(
        self: Tensor,
        comptime fmt: []const u8,
        options: std.fmt.FormatOptions,
        writer: anytype,
    ) !void {
        _ = fmt;
        _ = options;

        try writer.print("Tensor(", .{});
        for (self.shape, 0..) |dim, i| {
            if (i > 0) try writer.print(", ", .{});
            try writer.print("{}", .{dim});
        }
        try writer.print(") [", .{});

        const show_count = @min(self.data.len, 10);
        for (self.data[0..show_count], 0..) |val, i| {
            if (i > 0) try writer.print(", ", .{});
            try writer.print("{d:.3}", .{val});
        }

        if (self.data.len > 10) {
            try writer.print(", ... ({} more)", .{self.data.len - 10});
        }

        try writer.print("]", .{});
    }
};
