//
// Auto-generated from: Zig\Zig_Distributed_Systems.md
// Line: 251
// Language: zig
// Block ID: 1b1e4112
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Protocol Buffers 风格的定义
pub const GreeterService = struct {
    pub const HelloRequest = struct {
        name: []const u8,
    };

    pub const HelloReply = struct {
        message: []const u8,
    };

    pub fn sayHello(request: HelloRequest) !HelloReply {
        const message = try std.fmt.allocPrint(std.heap.page_allocator, "Hello, {s}!", .{request.name});
        return .{ .message = message };
    }
};

// 代码生成器 (简化)
pub fn generateService(comptime Service: type) type {
    return struct {
        pub fn serve(allocator: std.mem.Allocator, port: u16) !void {
            var server = try RpcServer.init(allocator, port);

            // 为每个方法生成处理器
            inline for (@typeInfo(Service).Struct.decls) |decl| {
                const method = @field(Service, decl.name);
                if (@TypeOf(method) == fn (anytype) anyerror!anytype) {
                    try server.register(decl.name, struct {
                        fn handler(payload: []const u8) ![]u8 {
                            // 反序列化请求
                            // 调用方法
                            // 序列化响应
                            _ = payload;
                            return &[_]u8{};
                        }
                    }.handler);
                }
            }

            try server.run();
        }
    };
}
