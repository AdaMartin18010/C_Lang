//
// Auto-generated from: Zig\Zig_Network_Programming.md
// Line: 135
// Language: zig
// Block ID: 36243ffd
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

const std = @import("std");
const net = std.net;

pub const TcpClient = struct {
    conn: net.tcp.Connection,

    pub fn connect(host: []const u8, port: u16) !TcpClient {
        const address = try net.Address.parseIp(host, port);
        const conn = try net.tcp.Connection.connect(address);

        return .{ .conn = conn };
    }

    pub fn close(self: *TcpClient) void {
        self.conn.close();
    }

    pub fn send(self: TcpClient, data: []const u8) !void {
        _ = try self.conn.write(data);
    }

    pub fn receive(self: TcpClient, buffer: []u8) !usize {
        return try self.conn.read(buffer);
    }
};

// 使用示例
pub fn clientExample() !void {
    var client = try TcpClient.connect("127.0.0.1", 8080);
    defer client.close();

    try client.send("Hello, Server!");

    var buffer: [1024]u8 = undefined;
    const bytes = try client.receive(&buffer);

    std.log.info("Received: {s}", .{buffer[0..bytes]});
}
