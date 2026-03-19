//
// Auto-generated from: Zig\Zig_Network_Programming.md
// Line: 356
// Language: zig
// Block ID: d530347e
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 基于事件循环的异步网络 (实验性)
const std = @import("std");

pub const AsyncServer = struct {
    loop: *std.event.Loop,
    address: std.net.Address,

    pub fn init(loop: *std.event.Loop, port: u16) !AsyncServer {
        const address = try std.net.Address.parseIp("0.0.0.0", port);
        return .{
            .loop = loop,
            .address = address,
        };
    }

    pub fn run(self: AsyncServer) !void {
        var server = std.x.net.tcp.Server.init(self.address);
        defer server.deinit();
        try server.listen(128);

        while (true) {
            const conn = try await server.accept();
            const handle_frame = async self.handleConnection(conn);
            // 不等待，继续接受新连接
        }
    }

    async fn handleConnection(self: AsyncServer, conn: std.x.net.tcp.Connection) !void {
        defer conn.close();

        var buffer: [4096]u8 = undefined;
        while (true) {
            const bytes = try await conn.read(&buffer);
            if (bytes == 0) break;

            _ = try await conn.write(buffer[0..bytes]);
        }
    }
};
