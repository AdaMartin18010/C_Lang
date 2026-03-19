//
// Auto-generated from: Zig\Zig_Network_Programming.md
// Line: 250
// Language: zig
// Block ID: ef7a9381
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

const std = @import("std");
const net = std.net;

pub const HttpClient = struct {
    allocator: std.mem.Allocator,

    pub fn get(self: HttpClient, url: []const u8) !Response {
        // 解析 URL
        const host_start = if (std.mem.startsWith(u8, url, "http://"))
            7
        else
            return error.UnsupportedProtocol;

        const path_start = std.mem.indexOf(u8, url[host_start..], "/") orelse url.len - host_start;
        const host = url[host_start .. host_start + path_start];
        const path = if (host_start + path_start >= url.len) "/" else url[host_start + path_start ..];

        // 连接到服务器
        const port: u16 = 80;
        const address = try net.Address.parseIp(host, port);
        const conn = try net.tcp.Connection.connect(address);
        defer conn.close();

        // 发送 HTTP 请求
        var request_buffer: [4096]u8 = undefined;
        const request = try std.fmt.bufPrint(&request_buffer,
            "GET {s} HTTP/1.1\r\n" ++
            "Host: {s}\r\n" ++
            "User-Agent: ZigHTTP/1.0\r\n" ++
            "Connection: close\r\n" ++
            "\r\n",
            .{ path, host }
        );

        _ = try conn.write(request);

        // 读取响应
        var response_buffer: [8192]u8 = undefined;
        var total_read: usize = 0;

        while (true) {
            const bytes = conn.read(response_buffer[total_read..]) catch break;
            if (bytes == 0) break;
            total_read += bytes;
        }

        // 解析响应
        return try self.parseResponse(response_buffer[0..total_read]);
    }

    fn parseResponse(self: HttpClient, data: []const u8) !Response {
        // 解析 HTTP 响应
        var lines = std.mem.split(u8, data, "\r\n");

        // 状态行
        const status_line = lines.next() orelse return error.InvalidResponse;
        const status_parts = std.mem.split(u8, status_line, " ");
        _ = status_parts.next();  // HTTP/1.1
        const status_str = status_parts.next() orelse return error.InvalidResponse;
        const status = try std.fmt.parseInt(u16, status_str, 10);

        // 解析头部
        var headers = std.StringHashMap([]const u8).init(self.allocator);
        errdefer headers.deinit();

        while (lines.next()) |line| {
            if (line.len == 0) break;  // 头部结束

            const colon = std.mem.indexOf(u8, line, ": ") orelse continue;
            const key = line[0..colon];
            const value = line[colon + 2 ..];

            try headers.put(key, value);
        }

        // 获取正文
        const body_start = std.mem.indexOf(u8, data, "\r\n\r\n") orelse data.len;
        const body = data[body_start + 4 ..];

        return Response{
            .status = status,
            .headers = headers,
            .body = try self.allocator.dupe(u8, body),
            .allocator = self.allocator,
        };
    }
};

pub const Response = struct {
    status: u16,
    headers: std.StringHashMap([]const u8),
    body: []const u8,
    allocator: std.mem.Allocator,

    pub fn deinit(self: *Response) void {
        self.headers.deinit();
        self.allocator.free(self.body);
    }
};
