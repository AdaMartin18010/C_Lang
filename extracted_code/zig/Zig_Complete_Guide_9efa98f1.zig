//
// Auto-generated from: Zig\Zig_Complete_Guide.md
// Line: 1359
// Language: zig
// Block ID: 9efa98f1
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

const std = @import("std");

pub fn main() !void {
    const allocator = std.heap.page_allocator;

    // 文件读取
    const cwd = std.fs.cwd();
    const data = try cwd.readFileAlloc(allocator, "input.txt", 1024 * 1024);
    defer allocator.free(data);
    std.debug.print("Read {} bytes\n", .{data.len});

    // 文件写入
    try cwd.writeFile(.{
        .sub_path = "output.txt",
        .data = "Hello, Zig!\n",
    });

    // 流式读取
    var file = try cwd.openFile("input.txt", .{});
    defer file.close();

    var buf_reader = std.io.bufferedReader(file.reader());
    var reader = buf_reader.reader();

    var line_buf: [1024]u8 = undefined;
    while (try reader.readUntilDelimiterOrEof(&line_buf, '\n')) |line| {
        std.debug.print("Line: {s}\n", .{line});
    }

    // 标准输入/输出
    const stdout = std.io.getStdOut().writer();
    try stdout.print("Enter name: ", .{});

    const stdin = std.io.getStdIn().reader();
    const name = try stdin.readUntilDelimiterAlloc(allocator, '\n', 100);
    defer allocator.free(name);

    try stdout.print("Hello, {s}!\n", .{name});

    // JSON 解析
    const json_str =
        \\{"name": "Zig", "version": "0.13"}
    ;

    const Package = struct {
        name: []const u8,
        version: []const u8,
    };

    const parsed = try std.json.parseFromSlice(Package, allocator, json_str, .{});
    defer parsed.deinit();

    std.debug.print("Package: {s} v{s}\n", .{ parsed.value.name, parsed.value.version });

    // 序列化
    var json_out = std.ArrayList(u8).init(allocator);
    defer json_out.deinit();

    try std.json.stringify(parsed.value, .{}, json_out.writer());
    std.debug.print("JSON: {s}\n", .{json_out.items});
}
