//
// Auto-generated from: Zig\2026_latest\README.md
// Line: 203
// Language: zig
// Block ID: 6bfe3e0f
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 2026年：更丰富的错误传播模式

const std = @import("std");

// 错误集合组合
const FileError = error{
    NotFound,
    PermissionDenied,
};

const NetworkError = error{
    ConnectionRefused,
    Timeout,
};

// 自动错误传播与转换
pub fn loadConfig() FileError!NetworkError!Config {
    const file = try std.fs.cwd().openFile("config.json", .{});
    defer file.close();

    const content = try file.readToEndAlloc(allocator, 1024 * 1024);
    defer allocator.free(content);

    // 自动错误转换和上下文附加
    return std.json.parseFromSlice(Config, allocator, content, .{})
        catch |err| return FileError.InvalidFormat;
}

// 使用 try 传播多重错误
pub fn initialize() !void {
    const config = try loadConfig();
    try connectToServer(config.server_address);
}
