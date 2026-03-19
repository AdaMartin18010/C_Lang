//
// Auto-generated from: Zig\Zig_Network_Programming.md
// Line: 47
// Language: zig
// Block ID: 3532b30c
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

const std = @import("std");
const net = std.net;

// 地址解析
pub fn addressExamples() !void {
    // IPv4
    const ipv4 = try net.Address.parseIp4("127.0.0.1", 8080);

    // IPv6
    const ipv6 = try net.Address.parseIp6("::1", 8080);

    // 从字符串自动识别
    const addr = try net.Address.parseIp("192.168.1.1", 80);

    _ = ipv4;
    _ = ipv6;
    _ = addr;
}
