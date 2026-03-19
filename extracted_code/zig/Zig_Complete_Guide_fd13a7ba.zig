//
// Auto-generated from: Zig\Zig_Complete_Guide.md
// Line: 846
// Language: zig
// Block ID: fd13a7ba
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

const std = @import("std");

// 定义错误集合
const FileError = error{
    NotFound,
    NoAccess,
    OutOfMemory,
    SystemResources,
};

const ParseError = error{
    InvalidSyntax,
    UnexpectedEnd,
};

// 组合错误
const MyError = FileError || ParseError;

// 函数返回错误
fn read_file(path: []const u8) FileError![]u8 {
    if (path.len == 0) return FileError.InvalidArgument;

    // 模拟文件读取
    const allocator = std.heap.page_allocator;
    const data = allocator.alloc(u8, 100) catch return FileError.OutOfMemory;

    @memcpy(data, "Hello from file");
    return data;
}

// try: 传播错误
fn process_file(path: []const u8) !void {
    const data = try read_file(path);  // 如果出错，立即返回
    defer std.heap.page_allocator.free(data);

    std.debug.print("File content: {s}\n", .{data});
}

// catch: 处理错误
fn safe_process_file(path: []const u8) void {
    const data = read_file(path) catch |err| {
        std.debug.print("Error reading file: {}\n", .{err});
        return;
    };
    defer std.heap.page_allocator.free(data);

    std.debug.print("File content: {s}\n", .{data});
}

// errdefer: 仅在错误时执行
fn may_fail_with_cleanup() !void {
    const allocator = std.heap.page_allocator;

    const resource = try allocator.create(Resource);
    errdefer allocator.destroy(resource);  // 只有出错时才销毁

    const data = try allocator.alloc(u8, 100);
    errdefer allocator.free(data);  // 只有出错时才释放

    // 正常流程
    resource.init(data);

    // 成功时手动清理
    resource.deinit();
    allocator.destroy(resource);
    allocator.free(data);
}

const Resource = struct {
    data: []u8,

    fn init(self: *Resource, d: []u8) void {
        self.data = d;
    }

    fn deinit(self: *Resource) void {
        _ = self;
    }
};

// switch 捕获错误
fn handle_error_detailed() void {
    const result = risky_operation();

    switch (result) {
        error.NotFound => std.debug.print("File not found\n", .{}),
        error.NoAccess => std.debug.print("Permission denied\n", .{}),
        error.OutOfMemory => std.debug.print("Out of memory\n", .{}),
        else => |e| std.debug.print("Other error: {}\n", .{e}),
    }
}

fn risky_operation() !void {
    return error.NotFound;
}

// if 错误捕获
pub fn main() void {
    if (read_file("test.txt")) |data| {
        std.debug.print("Success: {s}\n", .{data});
        std.heap.page_allocator.free(data);
    } else |err| {
        std.debug.print("Failed: {}\n", .{err});
    }

    // while 错误捕获
    while (retryable_operation()) |result| : (std.time.sleep(1_000_000)) {
        std.debug.print("Result: {}\n", .{result});
        if (result == 42) break;
    } else |err| {
        std.debug.print("Final error: {}\n", .{err});
    }
}

fn retryable_operation() !i32 {
    return 42;
}
