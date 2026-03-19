//
// Auto-generated from: Zig\Zig_Standard_Library_Deep_Dive.md
// Line: 1091
// Language: zig
// Block ID: ca4c0219
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

fn hmac_example() void {
    const key = "secret_key";
    const message = "Important message";

    // HMAC-SHA256
    var hmac = std.crypto.auth.hmac.sha2.HmacSha256.init(key);
    hmac.update(message);

    var auth_tag: [32]u8 = undefined;
    hmac.final(&auth_tag);

    std.log.info("HMAC: {s}", .{std.fmt.bytesToHex(auth_tag, .lower)});
}

fn pbkdf2_example() !void {
    const password = "user_password";
    const salt = "random_salt_123";
    const iterations = 100000;

    var derived_key: [32]u8 = undefined;

    try std.crypto.pwhash.pbkdf2(
        &derived_key,
        password,
        salt,
        iterations,
        std.crypto.auth.hmac.sha2.HmacSha256
    );

    std.log.info("Derived key: {s}", .{std.fmt.bytesToHex(derived_key, .lower)});
}
