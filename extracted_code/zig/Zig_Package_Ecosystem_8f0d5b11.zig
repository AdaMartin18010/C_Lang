//
// Auto-generated from: Zig\Zig_Package_Ecosystem.md
// Line: 472
// Language: zig
// Block ID: 8f0d5b11
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 简单的包服务器
const std = @import("std");
const http = std.http;

pub fn main() !void {
    var server = http.Server.init(std.heap.page_allocator, .{});
    defer server.deinit();

    try server.listen(try std.net.Address.parseIp("0.0.0.0", 8080));

    while (true) {
        const res = try server.accept(.{ .dynamic = 4096 });
        try handleRequest(res);
    }
}

fn handleRequest(res: *http.Server.Response) !void {
    const target = res.request.target;

    if (std.mem.startsWith(u8, target, "/packages/")) {
        const package_name = target[10..];
        try servePackage(res, package_name);
    } else if (std.mem.eql(u8, target, "/index.json")) {
        try serveIndex(res);
    } else {
        res.status = .not_found;
        try res.do();
    }
}

fn servePackage(res: *http.Server.Response, name: []const u8) !void {
    const packages_dir = "packages";
    const path = try std.fs.path.join(std.heap.page_allocator, &.{ packages_dir, name });

    const file = std.fs.cwd().openFile(path, .{}) catch {
        res.status = .not_found;
        try res.do();
        return;
    };
    defer file.close();

    res.status = .ok;
    res.transfer_encoding = .{ .content_length = try file.getEndPos() };
    try res.do();

    var buf: [4096]u8 = undefined;
    while (true) {
        const bytes = try file.read(&buf);
        if (bytes == 0) break;
        try res.writeAll(buf[0..bytes]);
    }
    try res.finish();
}

fn serveIndex(res: *http.Server.Response) !void {
    // 生成包索引
    const index =
        \\{
        \\  "packages": [
        \\    { "name": "private-lib", "version": "1.0.0", "url": "http://localhost:8080/packages/private-lib-1.0.0.tar.gz" }
        \\  ]
        \\}
    ;

    res.status = .ok;
    res.transfer_encoding = .{ .content_length = index.len };
    try res.do();
    try res.writeAll(index);
    try res.finish();
}
