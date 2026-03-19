//
// Auto-generated from: Zig\Zig_Standard_Library_Deep_Dive.md
// Line: 945
// Language: zig
// Block ID: 6d9270ee
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

fn udp_socket() !void {
    const allocator = std.heap.page_allocator;

    // 创建 UDP socket
    const socket = try std.posix.socket(
        std.posix.AF.INET,
        std.posix.SOCK.DGRAM,
        0
    );
    defer std.posix.close(socket);

    // 绑定地址
    const bind_addr = try std.net.Address.parseIp("0.0.0.0", 9876);
    try std.posix.bind(socket, &bind_addr.any, bind_addr.getOsSockLen());

    // 接收数据
    var recv_buf: [1024]u8 = undefined;
    var src_addr: std.net.Address = undefined;
    var src_addr_len: std.posix.socklen_t = @sizeOf(std.net.Address);

    const recv_len = try std.posix.recvfrom(
        socket,
        &recv_buf,
        0,
        &src_addr.any,
        &src_addr_len
    );

    std.log.info("Received {} bytes from {}", .{ recv_len, src_addr });

    // 发送响应
    const response = "ACK";
    _ = try std.posix.sendto(
        socket,
        response,
        0,
        &src_addr.any,
        src_addr_len
    );
}
