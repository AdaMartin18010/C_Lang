//
// Auto-generated from: Zig\Zig_Standard_Library_Deep_Dive.md
// Line: 861
// Language: zig
// Block ID: 0600c5c5
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

const std = @import("std");

fn tcp_server() !void {
    const allocator = std.heap.page_allocator;

    // 创建监听器
    const address = try std.net.Address.parseIp("127.0.0.1", 8080);
    var server = try address.listen(.{
        .reuse_address = true,
    });
    defer server.deinit();

    std.log.info("Server listening on {}", .{address});

    while (true) {
        const conn = try server.accept();
        std.log.info("Connection from {}", .{conn.address});

        // 在新线程或协程中处理
        _ = try std.Thread.spawn(.{}, handle_client, .{conn.stream});
    }
}

fn handle_client(stream: std.net.Stream) !void {
    defer stream.close();

    var buf: [1024]u8 = undefined;

    // 读取请求
    const n = try stream.read(&buf);
    std.log.info("Received: {s}", .{buf[0..n]});

    // 发送响应
    const response = "HTTP/1.1 200 OK\r\nContent-Length: 13\r\n\r\nHello, World!";
    _ = try stream.write(response);
}

fn tcp_client() !void {
    // 连接服务器
    const stream = try std.net.tcpConnectToHost(
        std.heap.page_allocator,
        "example.com",
        80
    );
    defer stream.close();

    // 发送 HTTP 请求
    const request = "GET / HTTP/1.1\r\nHost: example.com\r\nConnection: close\r\n\r\n";
    _ = try stream.write(request);

    // 读取响应
    var buf: [4096]u8 = undefined;
    const n = try stream.read(&buf);
    std.log.info("Response:\n{s}", .{buf[0..n]});
}

// 异步服务器 (使用事件循环)
fn async_tcp_server() !void {
    // Zig 的异步 I/O 需要事件循环支持
    // 这里展示简化版

    const address = try std.net.Address.parseIp("0.0.0.0", 8080);
    var server = try address.listen(.{});
    defer server.deinit();

    var buf: [1024]u8 = undefined;

    while (true) {
        const conn = try server.accept();
        defer conn.stream.close();

        // 非阻塞读取
        const n = try conn.stream.read(&buf);
        if (n > 0) {
            _ = try conn.stream.write("Echo: ");
            _ = try conn.stream.write(buf[0..n]);
        }
    }
}
