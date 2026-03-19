//
// Auto-generated from: 08_Zig_C_Connection\04_Translation_Validation\README.md
// Line: 418
// Language: zig
// Block ID: e86f06f8
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

/// C sds (Simple Dynamic String) 简化的 Zig 实现
const Sds = struct {
    allocator: std.mem.Allocator,
    buf: []u8,
    len: usize,

    const Self = @This();

    pub fn new(allocator: std.mem.Allocator, init: []const u8) !Self {
        var s = try allocator.alloc(u8, init.len * 2);
        @memcpy(s[0..init.len], init);
        return Self{
            .allocator = allocator,
            .buf = s,
            .len = init.len,
        };
    }

    pub fn free(self: *Self) void {
        self.allocator.free(self.buf);
        self.* = undefined;
    }

    pub fn cat(self: *Self, t: []const u8) !void {
        if (self.len + t.len > self.buf.len) {
            const new_cap = (self.len + t.len) * 2;
            self.buf = try self.allocator.realloc(self.buf, new_cap);
        }
        @memcpy(self.buf[self.len..][0..t.len], t);
        self.len += t.len;
    }

    pub fn slice(self: Self) []const u8 {
        return self.buf[0..self.len];
    }
};
