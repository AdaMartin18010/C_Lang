//
// Auto-generated from: Zig\Zig_Design_Patterns.md
// Line: 257
// Language: zig
// Block ID: 52f60cc0
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// RAII 模式

fn Resource(comptime T: type, comptime deinit_fn: fn (*T) void) type {
    return struct {
        value: T,

        pub fn init(val: T) @This() {
            return .{ .value = val };
        }

        pub fn deinit(self: *@This()) void {
            deinit_fn(&self.value);
        }

        pub fn get(self: *@This()) *T {
            return &self.value;
        }
    };
}

// 使用
const FileResource = Resource(std.fs.File, std.fs.File.close);

fn readFile(path: []const u8) !void {
    var file = FileResource.init(try std.fs.cwd().openFile(path, .{}));
    defer file.deinit();

    // 使用 file.get() 访问
    var buffer: [1024]u8 = undefined;
    _ = try file.get().read(&buffer);
}

// 作用域守卫
fn ScopeGuard(comptime T: type) type {
    return struct {
        resource: *T,
        active: bool,

        pub fn init(r: *T) @This() {
            return .{ .resource = r, .active = true };
        }

        pub fn dismiss(self: *@This()) void {
            self.active = false;
        }

        pub fn deinit(self: *@This(), comptime cleanup: fn (*T) void) void {
            if (self.active) {
                cleanup(self.resource);
            }
        }
    };
}
