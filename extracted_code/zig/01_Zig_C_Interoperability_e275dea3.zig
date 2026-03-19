//
// Auto-generated from: 07_Modern_Toolchain\13_Zig_C_Interop\01_Zig_C_Interoperability.md
// Line: 420
// Language: zig
// Block ID: e275dea3
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// tools/generate_header.zig

const std = @import("std");

const function_decls = [_][]const u8{
    "int zig_add(int a, int b);",
    "int zig_process_data(const char* data, size_t len);",
    "double zig_point_distance(const ZigPoint* a, const ZigPoint* b);",
};

const type_decls = [_][]const u8{
    "typedef struct { double x; double y; } ZigPoint;",
};

pub fn main() !void {
    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    defer _ = gpa.deinit();
    const allocator = gpa.allocator();

    var header = std.ArrayList(u8).init(allocator);
    defer header.deinit();

    const w = header.writer();

    try w.writeAll("#ifndef ZIG_LIB_H\n");
    try w.writeAll("#define ZIG_LIB_H\n\n");
    try w.writeAll("#include <stddef.h>\n\n");
    try w.writeAll("#ifdef __cplusplus\n");
    try w.writeAll('extern "C" {\n');
    try w.writeAll("#endif\n\n");

    try w.writeAll("/* Types */\n");
    for (type_decls) |decl| {
        try w.print("{s}\n", .{decl});
    }

    try w.writeAll("\n/* Functions */\n");
    for (function_decls) |decl| {
        try w.print("{s}\n", .{decl});
    }

    try w.writeAll("\n/* Global Variables */\n");
    try w.writeAll("extern int zig_global_counter;\n");

    try w.writeAll("\n#ifdef __cplusplus\n");
    try w.writeAll("}\n");
    try w.writeAll("#endif\n\n");
    try w.writeAll("#endif /* ZIG_LIB_H */\n");

    const stdout = std.io.getStdOut();
    try stdout.writeAll(header.items);
}
