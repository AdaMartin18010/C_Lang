//
// Auto-generated from: 08_Zig_C_Connection\05_Migration_Methodology\README.md
// Line: 390
// Language: zig
// Block ID: 827e64d5
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

/// 阶段 3: 完全 Zig 实现
const std = @import("std");

pub const Config = struct {
    allocator: std.mem.Allocator,
    values: std.StringHashMap([]const u8),

    const Self = @This();

    pub fn load(allocator: std.mem.Allocator, path: []const u8) !Self {
        var self = Self{
            .allocator = allocator,
            .values = std.StringHashMap([]const u8).init(allocator),
        };
        errdefer self.deinit();

        const content = try std.fs.cwd().readFileAlloc(allocator, path, 1024 * 1024);
        defer allocator.free(content);

        try self.parse(content);

        return self;
    }

    fn parse(self: *Self, content: []const u8) !void {
        var lines = std.mem.splitScalar(u8, content, '\n');
        while (lines.next()) |line| {
            const trimmed = std.mem.trim(u8, line, " \t\r");
            if (trimmed.len == 0 or trimmed[0] == '#') continue;

            if (std.mem.indexOfScalar(u8, trimmed, '=')) |sep| {
                const key = try self.allocator.dupe(u8, std.mem.trim(u8, trimmed[0..sep], " \t"));
                errdefer self.allocator.free(key);

                const value = try self.allocator.dupe(u8, std.mem.trim(u8, trimmed[sep + 1 ..], " \t"));
                errdefer self.allocator.free(value);

                try self.values.put(key, value);
            }
        }
    }

    // ... get, deinit 等
};
