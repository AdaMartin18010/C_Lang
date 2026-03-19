//
// Auto-generated from: Zig\Zig_Network_Programming.md
// Line: 180
// Language: zig
// Block ID: 1ed7094e
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

const std = @import("std");
const net = std.net;

pub const UdpSocket = struct {
    socket: net.udp.Socket,

    pub fn bind(port: u16) !UdpSocket {
        const address = try net.Address.parseIp("0.0.0.0", port);
        const socket = try net.udp.Socket.init(address);

        return .{ .socket = socket };
    }

    pub fn deinit(self: *UdpSocket) void {
        self.socket.deinit();
    }

    pub fn sendTo(self: UdpSocket, data: []const u8, dest: net.Address) !void {
        _ = try self.socket.sendTo(data, dest);
    }

    pub fn receiveFrom(self: UdpSocket, buffer: []u8) !struct { usize, net.Address } {
        return try self.socket.receiveFrom(buffer);
    }
};

// DNS 查询示例 (简化版)
const DNSHeader = extern struct {
    id: u16,
    flags: u16,
    questions: u16,
    answer_rrs: u16,
    authority_rrs: u16,
    additional_rrs: u16,
};

pub fn dnsQuery(domain: []const u8) !void {
    var socket = try UdpSocket.bind(0);
    defer socket.deinit();

    const dns_server = try net.Address.parseIp("8.8.8.8", 53);

    // 构建 DNS 查询包
    var query: [512]u8 = undefined;
    const header = DNSHeader{
        .id = 0x1234,
        .flags = 0x0100,  // 标准查询
        .questions = 1,
        .answer_rrs = 0,
        .authority_rrs = 0,
        .additional_rrs = 0,
    };

    @memcpy(query[0..12], std.mem.asBytes(&header));
    // 添加域名编码...

    try socket.sendTo(query[0..], dns_server);

    var response: [512]u8 = undefined;
    const len, _ = try socket.receiveFrom(&response);

    std.log.info("DNS response: {} bytes", .{len});
}
