//
// Auto-generated from: Zig\Zig_Standard_Library_Deep_Dive.md
// Line: 1169
// Language: zig
// Block ID: 7dfd0d50
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

fn json_handling() !void {
    const allocator = std.heap.page_allocator;

    // 解析 JSON
    const json_text =
        \\{
        \\  "name": "Zig",
        \\  "version": "0.13",
        \\  "features": ["fast", "safe", "simple"],
        \\  "downloads": {
        \\    "windows": "https://...",
        \\    "linux": "https://..."
        \\  }
        \\}
    ;

    const Package = struct {
        name: []const u8,
        version: []const u8,
        features: [][]const u8,
        downloads: struct {
            windows: []const u8,
            linux: []const u8,
        },
    };

    const parsed = try std.json.parseFromSlice(Package, allocator, json_text, .{});
    defer parsed.deinit();

    std.log.info("Package: {s} v{s}", .{ parsed.value.name, parsed.value.version });
    for (parsed.value.features) |feature| {
        std.log.info("  Feature: {s}", .{feature});
    }

    // 生成 JSON
    const new_pkg = Package{
        .name = "MyApp",
        .version = "1.0",
        .features = &.{ "async", "crypto", "web" },
        .downloads = .{
            .windows = "https://example.com/win",
            .linux = "https://example.com/linux",
        },
    };

    var output = std.ArrayList(u8).init(allocator);
    defer output.deinit();

    try std.json.stringify(new_pkg, .{ .whitespace = .indent_2 }, output.writer());
    std.log.info("JSON:\n{s}", .{output.items});

    // 流式解析 (大文件)
    // var scanner = std.json.Scanner.init(allocator, json_text);
    // while (try scanner.next()) |token| { ... }
}
