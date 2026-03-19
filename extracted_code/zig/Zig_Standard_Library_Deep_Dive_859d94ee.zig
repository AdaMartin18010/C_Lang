//
// Auto-generated from: Zig\Zig_Standard_Library_Deep_Dive.md
// Line: 550
// Language: zig
// Block ID: 859d94ee
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

const std = @import("std");

fn string_operations() !void {
    const allocator = std.heap.page_allocator;

    // Zig 字符串是 []const u8 或 []u8
    const literal = "Hello, Zig!";  // []const u8

    // 字符串长度
    std.log.info("Length: {}", .{literal.len});

    // 字符串切片
    const sub = literal[0..5];  // "Hello"
    _ = sub;

    // 字符串比较
    const equal = std.mem.eql(u8, "abc", "abc");
    const cmp = std.mem.order(u8, "abc", "def");
    std.log.info("Equal: {}, Order: {}", .{ equal, cmp });

    // 查找
    const idx = std.mem.indexOf(u8, literal, "Zig");
    if (idx) |pos| {
        std.log.info("Found at {}", .{pos});
    }

    const starts = std.mem.startsWith(u8, literal, "Hello");
    const ends = std.mem.endsWith(u8, literal, "Zig!");
    std.log.info("Starts: {}, Ends: {}", .{ starts, ends });

    // 包含
    const contains = std.mem.containsAtLeast(u8, literal, 1, "Zig");
    _ = contains;

    // 替换 (创建新字符串)
    const replaced = try std.mem.replaceOwned(u8, allocator, "Hello World", "World", "Zig");
    defer allocator.free(replaced);
    std.log.info("Replaced: {s}", .{replaced});

    // 分割
    const text = "apple,banana,orange";
    var it = std.mem.split(u8, text, ",");
    while (it.next()) |fruit| {
        std.log.info("Fruit: {s}", .{fruit});
    }

    // Tokenize (跳过空白)
    const sentence = "  Hello   World  Zig  ";
    var tok_it = std.mem.tokenize(u8, sentence, " ");
    while (tok_it.next()) |word| {
        std.log.info("Word: {s}", .{word});
    }

    // 连接字符串
    const parts = &[_][]const u8{ "Hello", " ", "World" };
    const joined = try std.mem.join(allocator, "-", parts);
    defer allocator.free(joined);
    std.log.info("Joined: {s}", .{joined});

    // 重复字符串
    const repeated = try allocator.alloc(u8, 10);
    defer allocator.free(repeated);
    @memset(repeated, '*');
    std.log.info("Repeated: {s}", .{repeated});

    // 修剪
    const padded = "  hello  ";
    const trimmed = std.mem.trim(u8, padded, " ");
    std.log.info("Trimmed: '{s}'", .{trimmed});

    // 大小写转换
    const upper = try std.ascii.allocUpperString(allocator, "hello");
    defer allocator.free(upper);
    std.log.info("Upper: {s}", .{upper});

    // 复制
    const copy = try allocator.dupe(u8, "original");
    defer allocator.free(copy);
}
