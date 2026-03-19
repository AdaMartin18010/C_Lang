//
// Auto-generated from: 08_Zig_C_Connection\01_Type_System_Mapping\02_Extern_Struct_Equivalence.md
// Line: 950
// Language: zig
// Block ID: cf03bdef
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig 代码 - 柔性数组等效模式
const std = @import("std");
const Allocator = std.mem.Allocator;

// 方案 1: 使用切片模拟柔性数组
const FlexibleSlice = extern struct {
    len: usize,
    // 实际数据存储在单独的分配中
};

const FlexibleArraySlice = struct {
    header: FlexibleSlice,
    data: []i32,  // 切片 = [ptr, len]

    fn create(allocator: Allocator, n: usize) !*FlexibleArraySlice {
        // 分配包含元数据和一个切片的结构体
        const self = try allocator.create(FlexibleArraySlice);
        errdefer allocator.destroy(self);

        self.header.len = n;
        self.data = try allocator.alloc(i32, n);
        errdefer allocator.free(self.data);

        for (0..n) |i| {
            self.data[i] = @intCast(i * i);
        }

        return self;
    }

    fn destroy(self: *FlexibleArraySlice, allocator: Allocator) void {
        allocator.free(self.data);
        allocator.destroy(self);
    }
};

// 方案 2: 单块分配 (更接近 C 语义)
const FlexibleArrayBlock = struct {
    const Header = extern struct {
        len: usize,
        // 数据紧随其后
    };

    // 分配一块内存: [Header][i32, i32, ...]
    fn create(allocator: Allocator, n: usize) !struct { ptr: [*]align(@alignOf(Header)) u8, header: *Header } {
        const header_size = @sizeOf(Header);
        const data_size = n * @sizeOf(i32);
        const total_size = header_size + data_size;

        // 按 Header 对齐要求分配
        const ptr = try allocator.alignedAlloc(u8, @alignOf(Header), total_size);

        const header: *Header = @ptrCast(ptr.ptr);
        header.len = n;

        // 获取数据指针
        const data_ptr: [*]i32 = @ptrCast(@alignCast(ptr.ptr + header_size));

        for (0..n) |i| {
            data_ptr[i] = @intCast(i * i);
        }

        return .{ .ptr = ptr.ptr, .header = header };
    }

    fn getDataPtr(header: *Header) [*]i32 {
        const ptr: [*]u8 = @ptrCast(header);
        return @ptrCast(@alignCast(ptr + @sizeOf(Header)));
    }

    fn destroy(ptr: [*]align(@alignOf(Header)) u8, allocator: Allocator, n: usize) void {
        const header_size = @sizeOf(Header);
        const data_size = n * @sizeOf(i32);
        const total_size = header_size + data_size;
        allocator.free(ptr[0..total_size]);
    }
};

// 方案 3: C 兼容的 extern struct (使用指针)
const CFlexibleArray = extern struct {
    len: usize,
    data: [*]i32,  // 指向可变数量元素的指针
};

pub fn main() void {
    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    defer _ = gpa.deinit();
    const allocator = gpa.allocator();

    // 使用方案 2
    const result = FlexibleArrayBlock.create(allocator, 5) catch unreachable;
    defer FlexibleArrayBlock.destroy(result.ptr, allocator, 5);

    std.debug.print("len = {d}\n", .{result.header.len});
    const data = FlexibleArrayBlock.getDataPtr(result.header);
    for (0..result.header.len) |i| {
        std.debug.print("data[{d}] = {d}\n", .{i, data[i]});
    }
}
