//
// Auto-generated from: Zig\Zig_Concurrency_Async.md
// Line: 729
// Language: zig
// Block ID: af7daa83
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// http_server.zig

const std = @import("std");
const net = std.net;
const Thread = std.Thread;

pub const Server = struct {
    allocator: std.mem.Allocator,
    address: net.Address,
    running: Atomic(bool),

    pub fn init(allocator: std.mem.Allocator, port: u16) !Server {
        const address = try net.Address.parseIp("0.0.0.0", port);
        return .{
            .allocator = allocator,
            .address = address,
            .running = Atomic(bool).init(false),
        };
    }

    pub fn start(self: *Server) !void {
        var listener = try net.tcp.Listener.init(self.address);
        defer listener.deinit();

        try listener.listen(128);
        self.running.store(true, .Release);

        std.log.info("Server listening on {}", .{self.address});

        while (self.running.load(.Acquire)) {
            const conn = listener.accept() catch |err| {
                std.log.err("Accept error: {}", .{err});
                continue;
            };

            // 为每个连接创建线程
            const thread = try Thread.spawn(.{}, handleConnection, .{ self.allocator, conn });
            thread.detach();
        }
    }

    pub fn stop(self: *Server) void {
        self.running.store(false, .Release);
    }
};

fn handleConnection(allocator: std.mem.Allocator, conn: net.tcp.Connection) void {
    defer conn.close();

    var buffer: [4096]u8 = undefined;
    const bytes_read = conn.read(&buffer) catch |err| {
        std.log.err("Read error: {}", .{err});
        return;
    };

    if (bytes_read == 0) return;

    // 解析请求
    const request = buffer[0..bytes_read];

    // 简单路由
    if (std.mem.startsWith(u8, request, "GET / ")) {
        const response =
            "HTTP/1.1 200 OK\r\n" ++
            "Content-Type: text/html\r\n" ++
            "Connection: close\r\n" ++
            "\r\n" ++
            "<h1>Hello from Zig!</h1>";

        _ = conn.write(response) catch {};
    } else if (std.mem.startsWith(u8, request, "GET /api/data ")) {
        const json = "{\"message\": \"Hello\", \"status\": \"ok\"}";
        const response = std.fmt.allocPrint(allocator,
            "HTTP/1.1 200 OK\r\n" ++
            "Content-Type: application/json\r\n" ++
            "Connection: close\r\n" ++
            "Content-Length: {}\r\n" ++
            "\r\n" ++
            "{s}",
            .{ json.len, json }
        ) catch return;
        defer allocator.free(response);

        _ = conn.write(response) catch {};
    } else {
        const response =
            "HTTP/1.1 404 Not Found\r\n" ++
            "Content-Type: text/plain\r\n" ++
            "Connection: close\r\n" ++
            "\r\n" ++
            "404 Not Found";

        _ = conn.write(response) catch {};
    }
}

// 主函数
pub fn main() !void {
    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    defer _ = gpa.deinit();
    const allocator = gpa.allocator();

    var server = try Server.init(allocator, 8080);
    try server.start();
}
