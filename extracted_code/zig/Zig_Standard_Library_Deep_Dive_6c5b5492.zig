//
// Auto-generated from: Zig\Zig_Standard_Library_Deep_Dive.md
// Line: 634
// Language: zig
// Block ID: 6c5b5492
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

fn formatting() !void {
    const allocator = std.heap.page_allocator;

    // 格式化到缓冲区
    var buf: [256]u8 = undefined;
    const printed = try std.fmt.bufPrint(&buf, "Name: {s}, Age: {}, Score: {d:.2}",
        .{ "Alice", 30, 95.5 });
    std.log.info("{s}", .{printed});

    // 动态分配格式化字符串
    const formatted = try std.fmt.allocPrint(allocator,
        "Binary: {b}, Hex: 0x{X}, Octal: {o}",
        .{ 255, 255, 255 });
    defer allocator.free(formatted);
    std.log.info("{s}", .{formatted});

    // 格式化选项
    // {:>10}  右对齐，宽度10
    // {:<10}  左对齐，宽度10
    // {:^10}  居中
    // {:0>5}  补零
    // {:.2}   小数点后2位

    const table = try std.fmt.allocPrint(allocator,
        "|{:<10}|{:>10}|{:>10}|\n|{:<10}|{:>10.2}|{:>10.2}|",
        .{ "Item", "Price", "Qty", "Apple", 1.50, 100 });
    defer allocator.free(table);
    std.log.info("\n{s}", .{table});

    // 自定义类型的格式化
    const Point = struct {
        x: f32,
        y: f32,

        pub fn format(self: @This(), comptime fmt: []const u8,
                      options: std.fmt.FormatOptions,
                      writer: anytype) !void {
            _ = fmt;
            _ = options;
            try writer.print("({d:.2}, {d:.2})", .{ self.x, self.y });
        }
    };

    const p = Point{ .x = 3.14159, .y = 2.71828 };
    const point_str = try std.fmt.allocPrint(allocator, "{}", .{p});
    defer allocator.free(point_str);
    std.log.info("Point: {s}", .{point_str});

    // 解析整数
    const parsed_int = try std.fmt.parseInt(i32, "-123", 10);
    const parsed_hex = try std.fmt.parseInt(u32, "FF", 16);
    std.log.info("Parsed: {}, {}", .{ parsed_int, parsed_hex });

    // 解析浮点数
    const parsed_float = try std.fmt.parseFloat(f64, "3.14159");
    std.log.info("Parsed float: {}", .{parsed_float});
}
