//
// Auto-generated from: 08_Zig_C_Connection\05_Migration_Methodology\01_Incremental_Migration_Patterns.md
// Line: 1723
// Language: zig
// Block ID: bf09731d
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// HTTP 客户端 Zig 实现示例

const std = @import("std");

pub const Client = struct {
    const Self = @This();

    allocator: std.mem.Allocator,
    connection_pool: ConnectionPool,

    pub const Request = struct {
        method: Method,
        url: Url,
        headers: std.StringHashMap([]const u8),
        body: ?[]const u8,

        pub const Method = enum {
            GET,
            POST,
            PUT,
            DELETE,
            PATCH,
        };
    };

    pub const Response = struct {
        status: u16,
        headers: std.StringHashMap([]const u8),
        body: []const u8,

        pub fn deinit(self: *Response, allocator: std.mem.Allocator) void {
            // 安全释放所有资源
            var header_iter = self.headers.iterator();
            while (header_iter.next()) |entry| {
                allocator.free(entry.key_ptr.*);
                allocator.free(entry.value_ptr.*);
            }
            self.headers.deinit();
            allocator.free(self.body);
        }
    };

    /// 异步请求（使用 Zig async）
    pub fn fetch(
        self: *Self,
        request: Request,
    ) !Response {
        // 从连接池获取连接
        const conn = try self.connection_pool.acquire(request.url);
        defer self.connection_pool.release(conn);

        // 构建请求
        var req_buffer = std.ArrayList(u8).init(self.allocator);
        defer req_buffer.deinit();

        const writer = req_buffer.writer();
        try writer.print("{s} {s} HTTP/1.1\r\n", .{
            @tagName(request.method),
            request.url.path,
        });
        try writer.print("Host: {s}\r\n", .{request.url.host});

        // 添加 headers
        var header_iter = request.headers.iterator();
        while (header_iter.next()) |entry| {
            try writer.print("{s}: {s}\r\n", .{
                entry.key_ptr.*,
                entry.value_ptr.*,
            });
        }

        if (request.body) |body| {
            try writer.print("Content-Length: {d}\r\n", .{body.len});
        }

        try writer.writeAll("\r\n");

        if (request.body) |body| {
            try writer.writeAll(body);
        }

        // 发送请求
        try conn.stream.writer().writeAll(req_buffer.items);

        // 解析响应
        return try self.parseResponse(conn);
    }

    fn parseResponse(self: *Self, conn: Connection) !Response {
        var reader = conn.stream.reader();

        // 读取状态行
        const status_line = try reader.readUntilDelimiterAlloc(
            self.allocator,
            '\n',
            1024
        );
        defer self.allocator.free(status_line);

        // 解析状态码
        const status = try parseStatus(status_line);

        // 读取 headers
        var headers = std.StringHashMap([]const u8).init(self.allocator);
        errdefer {
            var iter = headers.iterator();
            while (iter.next()) |entry| {
                self.allocator.free(entry.key_ptr.*);
                self.allocator.free(entry.value_ptr.*);
            }
            headers.deinit();
        }

        while (true) {
            const line = try reader.readUntilDelimiterAlloc(
                self.allocator,
                '\n',
                4096
            );
            defer self.allocator.free(line);

            if (line.len <= 1) break;  // 空行表示 headers 结束

            const colon_idx = std.mem.indexOfScalar(u8, line, ':')
                orelse continue;

            const key = try self.allocator.dupe(u8, line[0..colon_idx]);
            const value = std.mem.trim(u8, line[colon_idx + 1..], " \r\n");
            const value_copy = try self.allocator.dupe(u8, value);

            try headers.put(key, value_copy);
        }

        // 读取 body
        const content_length = if (headers.get("Content-Length")) |cl|
            try std.fmt.parseInt(usize, cl, 10)
        else
            0;

        const body = try self.allocator.alloc(u8, content_length);
        errdefer self.allocator.free(body);

        try reader.readNoEof(body);

        return Response{
            .status = status,
            .headers = headers,
            .body = body,
        };
    }
};

const ConnectionPool = struct {
    // 实现省略
    fn acquire(self: ConnectionPool, url: Url) !Connection { _ = self; _ = url; return error.Unimplemented; }
    fn release(self: ConnectionPool, conn: Connection) void { _ = self; _ = conn; }
};
const Connection = struct { stream: std.net.Stream };
const Url = struct { host: []const u8, path: []const u8 };

fn parseStatus(line: []const u8) !u16 {
    // 简单实现：假设格式 "HTTP/1.1 200 OK"
    const parts = std.mem.splitScalar(u8, line, ' ');
    var iter = parts;
    _ = iter.next();  // 跳过 "HTTP/1.1"
    const code_str = iter.next() orelse return error.InvalidStatus;
    return try std.fmt.parseInt(u16, code_str, 10);
}
