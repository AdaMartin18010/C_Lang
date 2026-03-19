//
// Auto-generated from: 07_Modern_Toolchain\13_Zig_C_Interop\01_Zig_C_Interoperability.md
// Line: 618
// Language: zig
// Block ID: 3560e402
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 自动翻译的原始代码 (translate-c 生成)
// pub const my_status_t = c_uint;
// pub const MY_STATUS_OK = @as(c_int, 0);
// pub const MY_STATUS_ERROR = @as(c_int, 1);

// 手动优化的版本
pub const Status = enum(c_uint) {
    ok = 0,
    error = 1,
    invalid_param = 2,
    out_of_memory = 3,

    pub fn toError(self: Status) !void {
        return switch (self) {
            .ok => {},
            .error => error.Generic,
            .invalid_param => error.InvalidParam,
            .out_of_memory => error.OutOfMemory,
            _ => error.Unknown,
        };
    }
};

// 原始结构体
// pub const my_buffer_t = extern struct {
//     data: [*c]u8,
//     len: usize,
//     capacity: usize,
// };

// 优化后的 Zig 风格 API
pub const Buffer = struct {
    inner: c.my_buffer_t,

    pub fn init(allocator: std.mem.Allocator, capacity: usize) !Buffer {
        const data = try allocator.alloc(u8, capacity);
        return .{
            .inner = .{
                .data = data.ptr,
                .len = 0,
                .capacity = capacity,
            },
        };
    }

    pub fn asSlice(self: Buffer) []u8 {
        return self.inner.data[0..self.inner.len];
    }

    pub fn append(self: *Buffer, data: []const u8) !void {
        if (self.inner.len + data.len > self.inner.capacity) {
            return error.NoSpace;
        }
        @memcpy(self.inner.data[self.inner.len..][0..data.len], data);
        self.inner.len += data.len;
    }
};
