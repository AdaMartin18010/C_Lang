//
// Auto-generated from: Zig\Zig_Concurrency_Async.md
// Line: 453
// Language: zig
// Block ID: ad9fd95c
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 手动事件循环 (实验性 API)
const Loop = std.event.Loop;

pub fn main() !void {
    var loop: Loop = undefined;
    try loop.initMultiThreaded(std.heap.page_allocator);
    defer loop.deinit();

    // 运行事件循环
    try loop.run();
}

// 基于事件循环的 TCP 服务器
const net = std.net;
const x = std.x.net;

async fn tcpServer() !void {
    const address = try net.Address.parseIp("127.0.0.1", 8080);
    var server = x.net.tcp.Server.init(address);
    defer server.deinit();

    try server.listen(128);

    while (true) {
        const conn = try await server.accept();
        const handle_frame = async handleConnection(conn);
        // 继续接受新连接
    }
}

async fn handleConnection(conn: x.net.tcp.Connection) !void {
    defer conn.close();

    var buffer: [1024]u8 = undefined;
    while (true) {
        const bytes_read = try await conn.read(&buffer);
        if (bytes_read == 0) break;

        _ = try await conn.write(buffer[0..bytes_read]);
    }
}
