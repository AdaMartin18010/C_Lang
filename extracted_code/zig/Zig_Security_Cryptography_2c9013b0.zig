//
// Auto-generated from: Zig\Zig_Security_Cryptography.md
// Line: 115
// Language: zig
// Block ID: 2c9013b0
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

const std = @import("std");
const crypto = std.crypto;

// SHA-256
pub fn sha256Example() void {
    var hash: [32]u8 = undefined;
    const message = "Hello, Zig!";

    crypto.hash.sha2.Sha256.hash(message, &hash, .{});

    // 打印哈希值
    std.debug.print("SHA-256: ", .{});
    for (hash) |byte| {
        std.debug.print("{x:0>2}", .{byte});
    }
    std.debug.print("\n", .{});
}

// SHA-3 / Keccak
pub fn sha3Example() void {
    var hash: [32]u8 = undefined;
    const message = "Hello, Zig!";

    crypto.hash.sha3.Sha3_256.hash(message, &hash, .{});
}

// BLAKE3 (高性能)
pub fn blake3Example() void {
    var hash: [32]u8 = undefined;
    const message = "Hello, Zig!";

    crypto.hash.Blake3.hash(message, &hash, .{});
}

// 增量哈希
pub fn incrementalHash() !void {
    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    defer _ = gpa.deinit();
    const allocator = gpa.allocator();

    var sha256 = crypto.hash.sha2.Sha256.init(.{});

    // 分块更新
    sha256.update("Hello, ");
    sha256.update("World");
    sha256.update("!");

    var hash: [32]u8 = undefined;
    sha256.final(&hash);

    _ = allocator;
}
