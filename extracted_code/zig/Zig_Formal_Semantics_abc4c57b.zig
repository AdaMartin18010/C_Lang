//
// Auto-generated from: Zig\Zig_Formal_Semantics.md
// Line: 590
// Language: zig
// Block ID: abc4c57b
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 错误语义示例

// 错误集定义
const FileError = error{
    NotFound,
    PermissionDenied,
    DiskFull,
};

const NetworkError = error{
    ConnectionRefused,
    Timeout,
};

// 错误集合并
const AppError = FileError || NetworkError;

// 错误传播
fn readConfig() FileError!Config {
    const file = try openFile("config.txt");  // 错误自动传播
    defer closeFile(file);

    const content = try readFile(file);
    return parseConfig(content);
}

// 错误处理
fn main() void {
    const config = readConfig() catch |err| switch (err) {
        error.NotFound => Config.default(),
        error.PermissionDenied => {
            logError("没有权限读取配置文件");
            return;
        },
        else => |e| {
            logError("未知错误: {}", .{e});
            return;
        },
    };

    // 使用 config
    _ = config;
}

// errdefer - 错误时回滚
fn atomicOperation(allocator: std.mem.Allocator) !void {
    const temp_file = try createTempFile();
    errdefer deleteFile(temp_file);  // 仅错误时执行

    const buffer = try allocator.alloc(u8, 1000);
    errdefer allocator.free(buffer);

    try writeData(temp_file, buffer);

    // 成功，重命名为正式文件
    try renameFile(temp_file, "data.txt");
}

const Config = struct {
    fn default() Config { return .{}; }
};
fn openFile(path: []const u8) FileError!File { _ = path; return undefined; }
fn closeFile(f: File) void { _ = f; }
fn readFile(f: File) FileError![]u8 { _ = f; return undefined; }
fn parseConfig(c: []u8) Config { _ = c; return .{}; }
fn logError(comptime fmt: []const u8, args: anytype) void { _ = fmt; _ = args; }
fn createTempFile() FileError!File { return undefined; }
fn deleteFile(f: File) void { _ = f; }
fn writeData(f: File, b: []u8) !void { _ = f; _ = b; }
fn renameFile(from: File, to: []const u8) !void { _ = from; _ = to; }
const File = opaque {};
