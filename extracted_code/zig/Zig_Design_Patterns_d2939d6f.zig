//
// Auto-generated from: Zig\Zig_Design_Patterns.md
// Line: 106
// Language: zig
// Block ID: d2939d6f
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 使用类型系统编码状态机

const std = @import("std");

// 未连接状态
const Disconnected = struct {};

// 已连接状态
const Connected = struct {
    socket: std.net.tcp.Connection,
};

// 状态转换的 Connection
fn Connection(comptime State: type) type {
    return struct {
        state: State,

        pub fn init() Connection(Disconnected) {
            return .{ .state = .{} };
        }

        // 只能在 Disconnected 状态下调用
        pub fn connect(self: Connection(Disconnected), address: std.net.Address) !Connection(Connected) {
            const socket = try std.net.tcp.Connection.connect(address);
            return .{ .state = .{ .socket = socket } };
        }

        // 只能在 Connected 状态下调用
        pub fn send(self: Connection(Connected), data: []const u8) !void {
            _ = try self.state.socket.write(data);
        }

        pub fn receive(self: Connection(Connected), buffer: []u8) !usize {
            return try self.state.socket.read(buffer);
        }

        pub fn disconnect(self: Connection(Connected)) Connection(Disconnected) {
            self.state.socket.close();
            return .{ .state = .{} };
        }
    };
}

// 使用
pub fn example() !void {
    var conn = Connection(Disconnected).init();

    // 类型错误: conn.send(...)  // 编译错误！

    conn = try conn.connect(try std.net.Address.parseIp("127.0.0.1", 8080));

    try conn.send("Hello");  // OK

    conn = conn.disconnect();
    // conn.send(...)  // 编译错误！
}
