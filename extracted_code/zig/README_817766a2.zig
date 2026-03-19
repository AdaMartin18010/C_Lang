//
// Auto-generated from: Zig\README.md
// Line: 1699
// Language: zig
// Block ID: 817766a2
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 类型安全的资源管理
fn ResourceManager(comptime T: type) type {
    return struct {
        resources: std.StringHashMap(T),
        allocator: std.mem.Allocator,

        const Self = @This();

        pub fn init(allocator: std.mem.Allocator) Self {
            return .{
                .resources = std.StringHashMap(T).init(allocator),
                .allocator = allocator,
            };
        }

        pub fn deinit(self: *Self) void {
            var iter = self.resources.iterator();
            while (iter.next()) |entry| {
                self.allocator.free(entry.key_ptr.*);
                entry.value_ptr.deinit();
            }
            self.resources.deinit();
        }

        pub fn load(self: *Self, path: []const u8) !*T {
            if (self.resources.getPtr(path)) |resource| {
                return resource;
            }

            const key = try self.allocator.dupe(u8, path);
            errdefer self.allocator.free(key);

            const resource = try T.load(self.allocator, path);
            errdefer resource.deinit();

            try self.resources.put(key, resource);
            return self.resources.getPtr(key).?;
        }
    };
}
