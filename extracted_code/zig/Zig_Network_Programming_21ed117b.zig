//
// Auto-generated from: Zig\Zig_Network_Programming.md
// Line: 74
// Language: zig
// Block ID: 21ed117b
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

const std = @import("std");
const net = std.net;
const Thread = std.Thread;

pub const TcpServer = struct {
    allocator: std.mem.Allocator,
    listener: net.tcp.Listener,

    pub fn init(allocator: std.mem.Allocator, port: u16) !TcpServer {
        const address = try net.Address.parseIp("0.0.0.0", port);
        var listener = try net.tcp.Listener.init(address);
        try listener.listen(128);

        return .{
            .allocator = allocator,
            .listener = listener,
        };
    }

    pub fn deinit(self: *TcpServer) void {
        self.listener.deinit();
    }

    pub fn run(self: *TcpServer) !void {
        std.log.info("Server listening on port {}", .{self.listener.address.getPort()});

        while (true) {
            const conn = try self.listener.accept();

            // 为每个连接创建线程
            const thread = try Thread.spawn(.{}, handleConnection, .{self.allocator, conn});
            thread.detach();
        }
    }
};

fn handleConnection(allocator: std.mem.Allocator, conn: net.tcp.Connection) void {
    defer conn.close();

    var buffer: [4096]u8 = undefined;

    while (true) {
        const bytes_read = conn.read(&buffer) catch |err| {
            std.log.err("Read error: {}", .{err});
            return;
        };

        if (bytes_read == 0) return;  // 连接关闭

        // 回声
        _ = conn.write(buffer[0..bytes_read]) catch |err| {
            std.log.err("Write error: {}", .{err});
            return;
        };
    }
}
