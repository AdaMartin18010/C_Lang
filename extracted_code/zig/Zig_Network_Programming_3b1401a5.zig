//
// Auto-generated from: Zig\Zig_Network_Programming.md
// Line: 404
// Language: zig
// Block ID: 3b1401a5
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

const std = @import("std");
const crypto = std.crypto;

pub const WebSocket = struct {
    conn: std.net.tcp.Connection,

    pub fn handshake(allocator: std.mem.Allocator, conn: std.net.tcp.Connection, request: []const u8) !WebSocket {
        // 解析 Sec-WebSocket-Key
        const key_prefix = "Sec-WebSocket-Key: ";
        const key_start = std.mem.indexOf(u8, request, key_prefix) orelse return error.InvalidHandshake;
        const key_end = std.mem.indexOf(u8, request[key_start..], "\r\n") orelse return error.InvalidHandshake;
        const client_key = request[key_start + key_prefix.len .. key_start + key_end];

        // 计算 accept key
        const magic = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
        var hash_input: [256]u8 = undefined;
        const hash_len = try std.fmt.bufPrint(&hash_input, "{s}{s}", .{ client_key, magic });

        var hash: [20]u8 = undefined;
        crypto.hash.Sha1.hash(hash_input[0..hash_len.len], &hash, .{});

        var accept_key: [28]u8 = undefined;
        _ = std.base64.standard.Encoder.encode(&accept_key, &hash);

        // 发送响应
        var response: [512]u8 = undefined;
        const response_str = try std.fmt.bufPrint(&response,
            "HTTP/1.1 101 Switching Protocols\r\n" ++
            "Upgrade: websocket\r\n" ++
            "Connection: Upgrade\r\n" ++
            "Sec-WebSocket-Accept: {s}\r\n" ++
            "\r\n",
            .{&accept_key}
        );

        _ = try conn.write(response_str);

        return .{ .conn = conn };
    }

    pub fn sendText(self: WebSocket, text: []const u8) !void {
        const frame = try self.buildFrame(0x1, text);
        _ = try self.conn.write(frame);
    }

    pub fn sendBinary(self: WebSocket, data: []const u8) !void {
        const frame = try self.buildFrame(0x2, data);
        _ = try self.conn.write(frame);
    }

    fn buildFrame(self: WebSocket, opcode: u8, payload: []const u8) ![]const u8 {
        _ = self;

        var frame: [65536]u8 = undefined;
        var pos: usize = 0;

        // FIN=1, opcode
        frame[pos] = 0x80 | opcode;
        pos += 1;

        // Mask=0, payload length
        if (payload.len < 126) {
            frame[pos] = @intCast(payload.len);
            pos += 1;
        } else if (payload.len < 65536) {
            frame[pos] = 126;
            pos += 1;
            frame[pos] = @intCast(payload.len >> 8);
            frame[pos + 1] = @intCast(payload.len & 0xFF);
            pos += 2;
        } else {
            frame[pos] = 127;
            pos += 1;
            // 8 字节长度
            const len = payload.len;
            std.mem.writeIntBig(u64, frame[pos..][0..8], len);
            pos += 8;
        }

        // 复制 payload
        @memcpy(frame[pos..][0..payload.len], payload);
        pos += payload.len;

        return frame[0..pos];
    }

    pub fn close(self: WebSocket) void {
        _ = self.conn.write(&[_]u8{ 0x88, 0x00 }) catch {};
        self.conn.close();
    }
};
