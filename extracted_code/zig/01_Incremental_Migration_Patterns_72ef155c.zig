//
// Auto-generated from: 08_Zig_C_Connection\05_Migration_Methodology\01_Incremental_Migration_Patterns.md
// Line: 434
// Language: zig
// Block ID: 72ef155c
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 示例：crypto 模块（叶节点）迁移

// C 原始代码（crypto_sha256.c）
/*
typedef struct {
    uint32_t state[8];
    uint64_t bitcount;
    uint8_t buffer[64];
    size_t buffer_used;
} sha256_ctx;

void sha256_init(sha256_ctx* ctx);
int sha256_update(sha256_ctx* ctx, const uint8_t* data, size_t len);
void sha256_final(sha256_ctx* ctx, uint8_t hash[32]);
*/

// Zig 迁移实现（crypto.zig）
const std = @import("std");

pub const Sha256 = struct {
    const Self = @This();
    const block_size = 64;
    const digest_size = 32;

    state: [8]u32,
    bitcount: u64,
    buffer: [64]u8,
    buffer_used: usize,

    pub fn init() Self {
        return .{
            .state = [8]u32{
                0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
                0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19,
            },
            .bitcount = 0,
            .buffer = undefined,
            .buffer_used = 0,
        };
    }

    pub fn update(self: *Self, data: []const u8) void {
        // 类型安全、边界安全的实现
        var i: usize = 0;
        while (i < data.len) {
            const space = block_size - self.buffer_used;
            const to_copy = @min(space, data.len - i);

            @memcpy(
                self.buffer[self.buffer_used..self.buffer_used + to_copy],
                data[i..i + to_copy]
            );

            self.buffer_used += to_copy;
            i += to_copy;

            if (self.buffer_used == block_size) {
                self.processBlock();
                self.buffer_used = 0;
            }
        }
        self.bitcount += @as(u64, data.len) * 8;
    }

    pub fn final(self: *Self, out: *[digest_size]u8) void {
        // 安全地完成哈希计算
        self.buffer[self.buffer_used] = 0x80;
        self.buffer_used += 1;

        // ... 填充逻辑 ...

        // 输出大端序结果
        for (0..8) |i| {
            std.mem.writeInt(u32, out[i*4..][0..4], self.state[i], .big);
        }
    }

    fn processBlock(self: *Self) void {
        // 处理单个块
        // 编译时展开循环优化
        var w: [64]u32 = undefined;

        comptime var i: usize = 0;
        inline while (i < 16) : (i += 1) {
            w[i] = std.mem.readInt(u32, self.buffer[i*4..][0..4], .big);
        }
        // ... 其余处理 ...
    }
};

// C ABI 兼容接口
export fn sha256_init(ctx: ?*anyopaque) void {
    const self = @as(*Sha256, @alignCast(@ptrCast(ctx.?)));
    self.* = Sha256.init();
}

export fn sha256_update(
    ctx: ?*anyopaque,
    data: ?[*]const u8,
    len: usize
) c_int {
    if (ctx == null or data == null) return -1;
    const self = @as(*Sha256, @alignCast(@ptrCast(ctx.?)));
    self.update(data.?[0..len]);
    return 0;
}

export fn sha256_final(
    ctx: ?*anyopaque,
    hash: ?[*]u8
) void {
    const self = @as(*Sha256, @alignCast(@ptrCast(ctx.?)));
    const out = @as(*[32]u8, @ptrCast(hash.?));
    self.final(out);
}
