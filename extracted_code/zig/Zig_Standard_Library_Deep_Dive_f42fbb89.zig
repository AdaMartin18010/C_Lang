//
// Auto-generated from: Zig\Zig_Standard_Library_Deep_Dive.md
// Line: 1050
// Language: zig
// Block ID: f42fbb89
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

const std = @import("std");

fn hashing_examples() void {
    const data = "Hello, World!";

    // SHA-256
    var sha256 = std.crypto.hash.sha2.Sha256.init(.{});
    sha256.update(data);

    var sha256_digest: [32]u8 = undefined;
    sha256.final(&sha256_digest);

    std.log.info("SHA-256: {s}", .{std.fmt.bytesToHex(sha256_digest, .lower)});

    // SHA-512
    var sha512 = std.crypto.hash.sha2.Sha512.init(.{});
    sha512.update(data);

    var sha512_digest: [64]u8 = undefined;
    sha512.final(&sha512_digest);

    std.log.info("SHA-512: {s}", .{std.fmt.bytesToHex(sha512_digest, .lower)});

    // Blake3
    var blake3 = std.crypto.hash.Blake3.init(.{});
    blake3.update(data);

    var blake3_digest: [32]u8 = undefined;
    blake3.final(&blake3_digest);

    std.log.info("Blake3: {s}", .{std.fmt.bytesToHex(blake3_digest, .lower)});

    // 单次哈希便捷函数
    const quick_sha256 = std.crypto.hash.sha2.Sha256.hash(data, .{});
    std.log.info("Quick SHA-256: {s}", .{std.fmt.bytesToHex(quick_sha256, .lower)});
}
