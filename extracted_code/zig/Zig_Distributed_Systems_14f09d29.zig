//
// Auto-generated from: Zig\Zig_Distributed_Systems.md
// Line: 87
// Language: zig
// Block ID: 14f09d29
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

const std = @import("std");
const net = std.net;

// RPC 消息格式
pub const RpcMessage = struct {
    id: u64,
    method: []const u8,
    payload: []const u8,
};

pub const RpcResponse = struct {
    id: u64,
    success: bool,
    result: []const u8,
    error_message: ?[]const u8,
};

// 序列化/反序列化
pub const Serializer = struct {
    pub fn serialize(allocator: std.mem.Allocator, msg: RpcMessage) ![]u8 {
        // 简单 TLV 格式
        const method_len = msg.method.len;
        const payload_len = msg.payload.len;
        const total_len = 8 + 4 + method_len + 4 + payload_len;

        var buffer = try allocator.alloc(u8, total_len);
        var pos: usize = 0;

        // ID (8 bytes)
        std.mem.writeIntLittle(u64, buffer[pos..][0..8], msg.id);
        pos += 8;

        // Method length + data
        std.mem.writeIntLittle(u32, buffer[pos..][0..4], @intCast(method_len));
        pos += 4;
        @memcpy(buffer[pos..][0..method_len], msg.method);
        pos += method_len;

        // Payload length + data
        std.mem.writeIntLittle(u32, buffer[pos..][0..4], @intCast(payload_len));
        pos += 4;
        @memcpy(buffer[pos..][0..payload_len], msg.payload);

        return buffer;
    }

    pub fn deserialize(allocator: std.mem.Allocator, data: []const u8) !RpcMessage {
        var pos: usize = 0;

        const id = std.mem.readIntLittle(u64, data[pos..][0..8]);
        pos += 8;

        const method_len = std.mem.readIntLittle(u32, data[pos..][0..4]);
        pos += 4;
        const method = try allocator.dupe(u8, data[pos..][0..method_len]);
        pos += method_len;

        const payload_len = std.mem.readIntLittle(u32, data[pos..][0..4]);
        pos += 4;
        const payload = try allocator.dupe(u8, data[pos..][0..payload_len]);

        return .{
            .id = id,
            .method = method,
            .payload = payload,
        };
    }
};

// RPC 服务器
pub const RpcServer = struct {
    allocator: std.mem.Allocator,
    handlers: std.StringHashMap(Handler),
    listener: net.tcp.Listener,

    const Handler = *const fn ([]const u8) anyerror![]u8;

    pub fn init(allocator: std.mem.Allocator, port: u16) !RpcServer {
        const address = try net.Address.parseIp("0.0.0.0", port);
        var listener = try net.tcp.Listener.init(address);
        try listener.listen(128);

        return .{
            .allocator = allocator,
            .handlers = std.StringHashMap(Handler).init(allocator),
            .listener = listener,
        };
    }

    pub fn register(self: *RpcServer, method: []const u8, handler: Handler) !void {
        try self.handlers.put(method, handler);
    }

    pub fn run(self: *RpcServer) !void {
        while (true) {
            const conn = try self.listener.accept();
            const thread = try std.Thread.spawn(.{}, handleConnection, .{ self, conn });
            thread.detach();
        }
    }

    fn handleConnection(self: *RpcServer, conn: net.tcp.Connection) void {
        defer conn.close();

        var buffer: [4096]u8 = undefined;

        while (true) {
            const bytes = conn.read(&buffer) catch break;
            if (bytes == 0) break;

            // 解析请求
            const request = Serializer.deserialize(self.allocator, buffer[0..bytes]) catch continue;
            defer {
                self.allocator.free(request.method);
                self.allocator.free(request.payload);
            }

            // 查找处理器
            const handler = self.handlers.get(request.method) orelse {
                const error_response = RpcResponse{
                    .id = request.id,
                    .success = false,
                    .result = &[_]u8{},
                    .error_message = "Method not found",
                };
                const response_data = Serializer.serializeResponse(self.allocator, error_response) catch continue;
                defer self.allocator.free(response_data);
                _ = conn.write(response_data) catch {};
                continue;
            };

            // 执行处理
            const result = handler(request.payload) catch |err| {
                const error_response = RpcResponse{
                    .id = request.id,
                    .success = false,
                    .result = &[_]u8{},
                    .error_message = @errorName(err),
                };
                const response_data = Serializer.serializeResponse(self.allocator, error_response) catch continue;
                defer self.allocator.free(response_data);
                _ = conn.write(response_data) catch {};
                continue;
            };
            defer self.allocator.free(result);

            // 发送响应
            const response = RpcResponse{
                .id = request.id,
                .success = true,
                .result = result,
                .error_message = null,
            };
            const response_data = Serializer.serializeResponse(self.allocator, response) catch continue;
            defer self.allocator.free(response_data);
            _ = conn.write(response_data) catch {};
        }
    }
};
