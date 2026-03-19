//
// Auto-generated from: Zig\Zig_Standard_Library_Deep_Dive.md
// Line: 1229
// Language: zig
// Block ID: 89c8f821
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

fn other_formats() !void {
    // CSV
    const csv_data = "name,age,city\nAlice,30,NYC\nBob,25,LA";
    var lines_it = std.mem.split(u8, csv_data, "\n");

    const header_line = lines_it.next() orelse "";
    var headers_it = std.mem.split(u8, header_line, ",");

    while (lines_it.next()) |line| {
        var fields_it = std.mem.split(u8, line, ",");
        while (fields_it.next()) |field| {
            std.log.info("Field: {s}", .{field});
        }
    }

    // Hex 编码/解码
    const data = "Hello";
    const hex_encoded = std.fmt.bytesToHex(data, .lower);
    std.log.info("Hex: {s}", .{hex_encoded});

    // Base64
    const allocator = std.heap.page_allocator;
    const base64_encoder = std.base64.standard.Encoder;
    const base64_buf = try allocator.alloc(u8, base64_encoder.calcSize(data.len));
    defer allocator.free(base64_buf);

    const base64_encoded = base64_encoder.encode(base64_buf, data);
    std.log.info("Base64: {s}", .{base64_encoded});
}
