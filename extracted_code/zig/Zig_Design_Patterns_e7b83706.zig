//
// Auto-generated from: Zig\Zig_Design_Patterns.md
// Line: 317
// Language: zig
// Block ID: e7b83706
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 错误传播
fn mayFail() !void {
    try operation1();
    try operation2();
}

// 错误恢复
fn withRecovery() void {
    doPrimary() catch |err| switch (err) {
        error.NotFound => doFallback(),
        error.OutOfMemory => handleOOM(),
        else => |e| return e,
    };
}

// 错误累积
fn validate(input: []const u8) ValidationError!void {
    var errors = std.ArrayList([]const u8).init(allocator);
    defer errors.deinit();

    if (input.len < 8) try errors.append("Too short");
    if (!hasUpper(input)) try errors.append("No uppercase");
    if (!hasDigit(input)) try errors.append("No digit");

    if (errors.items.len > 0) {
        return ValidationError{ .messages = errors.toOwnedSlice() };
    }
}

// 可选值链
fn findUser(db: *Database, id: u64) ?User {
    return db.findById(id) orelse
           db.findByEmail(id) orelse
           db.findByPhone(id);
}

// 错误和可选值转换
fn getOrDefault(map: *Map, key: []const u8, default: Value) Value {
    return map.get(key) catch default;
}
