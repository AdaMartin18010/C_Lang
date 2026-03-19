//
// Auto-generated from: Zig\Zig_Standard_Library_Deep_Dive.md
// Line: 990
// Language: zig
// Block ID: 5bf0156f
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 使用标准库的 HTTP 支持 (Zig 0.12+)
fn http_client() !void {
    const allocator = std.heap.page_allocator;

    var client = std.http.Client{ .allocator = allocator };
    defer client.deinit();

    const uri = try std.Uri.parse("https://api.example.com/data");

    var server_header_buffer: [4096]u8 = undefined;
    var request = try client.open(.GET, uri, .{
        .server_header_buffer = &server_header_buffer,
    });
    defer request.deinit();

    request.headers.content_type = .{ .override = "application/json" };

    try request.send();
    try request.finish();
    try request.wait();

    const body = try request.reader().readAllAlloc(allocator, 1024 * 1024);
    defer allocator.free(body);

    std.log.info("Response: {s}", .{body});
}

fn http_server() !void {
    const allocator = std.heap.page_allocator;
    const address = try std.net.Address.parseIp("127.0.0.1", 8080);

    var server = try address.listen(.{});
    defer server.deinit();

    std.log.info("HTTP server on http://{}/", .{address});

    while (true) {
        const conn = try server.accept();

        // 简单 HTTP 响应
        const response =
            \\HTTP/1.1 200 OK\r
            \\Content-Type: text/html\r
            \\Content-Length: 44\r
            \\r
            \\<html><body><h1>Hello from Zig!</h1></body></html>
        ;
        _ = try conn.stream.write(response);
        conn.stream.close();
    }
}
