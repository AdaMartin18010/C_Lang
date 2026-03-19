//
// Auto-generated from: Zig\Zig_Design_Patterns.md
// Line: 364
// Language: zig
// Block ID: d3425906
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

const Request = struct {
    method: []const u8,
    url: []const u8,
    headers: std.StringHashMap([]const u8),
    body: ?[]const u8,

    pub const Builder = struct {
        method: []const u8 = "GET",
        url: []const u8 = "",
        headers: std.StringHashMap([]const u8),
        body: ?[]const u8 = null,
        allocator: std.mem.Allocator,

        pub fn init(allocator: std.mem.Allocator) Builder {
            return .{
                .headers = std.StringHashMap([]const u8).init(allocator),
                .allocator = allocator,
            };
        }

        pub fn method(self: *Builder, m: []const u8) *Builder {
            self.method = m;
            return self;
        }

        pub fn url(self: *Builder, u: []const u8) *Builder {
            self.url = u;
            return self;
        }

        pub fn header(self: *Builder, key: []const u8, value: []const u8) !*Builder {
            try self.headers.put(key, value);
            return self;
        }

        pub fn body(self: *Builder, b: []const u8) *Builder {
            self.body = b;
            return self;
        }

        pub fn build(self: *Builder) Request {
            return .{
                .method = self.method,
                .url = self.url,
                .headers = self.headers,
                .body = self.body,
            };
        }
    };
};

// 使用
fn builderExample(allocator: std.mem.Allocator) !void {
    var builder = Request.Builder.init(allocator);

    const request = builder
        .method("POST")
        .url("/api/users")
        .header("Content-Type", "application/json") catch unreachable
        .header("Authorization", "Bearer token") catch unreachable
        .body("{\"name\":\"John\"}")
        .build();

    _ = request;
}
