//
// Auto-generated from: Zig\Zig_WebAssembly.md
// Line: 278
// Language: zig
// Block ID: adba35f1
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// cli.zig - WASI 命令行程序

const std = @import("std");

pub fn main() !void {
    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    defer _ = gpa.deinit();
    const allocator = gpa.allocator();

    const args = try std.process.argsAlloc(allocator);
    defer std.process.argsFree(allocator, args);

    if (args.len < 2) {
        try std.io.getStdErr().writeAll("Usage: program <file>\n");
        return error.InvalidArguments;
    }

    const content = std.fs.cwd().readFileAlloc(allocator, args[1], 1024 * 1024) catch |err| {
        try std.io.getStdErr().writer().print("Error: {}\n", .{err});
        return err;
    };
    defer allocator.free(content);

    const word_count = count_words(content);
    const line_count = count_lines(content);

    try std.io.getStdOut().writer().print(
        "Words: {} Lines: {} Bytes: {}\n",
        .{ word_count, line_count, content.len }
    );
}

fn count_words(text: []const u8) usize {
    var count: usize = 0;
    var in_word = false;
    for (text) |c| {
        if (std.ascii.isWhitespace(c)) {
            if (in_word) count += 1;
            in_word = false;
        } else {
            in_word = true;
        }
    }
    if (in_word) count += 1;
    return count;
}

fn count_lines(text: []const u8) usize {
    var count: usize = 1;
    for (text) |c| {
        if (c == '\n') count += 1;
    }
    return count;
}
