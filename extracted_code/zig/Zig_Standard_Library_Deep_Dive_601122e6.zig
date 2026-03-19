//
// Auto-generated from: Zig\Zig_Standard_Library_Deep_Dive.md
// Line: 700
// Language: zig
// Block ID: 601122e6
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

fn file_operations() !void {
    const allocator = std.heap.page_allocator;

    // 获取当前工作目录
    const cwd = std.fs.cwd();

    // 写入文件
    {
        const file = try cwd.createFile("test.txt", .{ .truncate = true });
        defer file.close();

        const writer = file.writer();
        try writer.print("Hello, {s}!\n", .{"Zig"});
        try writer.print("Line 2\n");
        try writer.writeAll("Line 3\n");
    }

    // 追加模式
    {
        const file = try cwd.openFile("test.txt", .{ .mode = .write_only });
        defer file.close();

        try file.seekFromEnd(0);
        const writer = file.writer();
        try writer.print("Appended line\n");
    }

    // 读取整个文件
    const content = try cwd.readFileAlloc(allocator, "test.txt", 1024 * 1024);
    defer allocator.free(content);
    std.log.info("Content:\n{s}", .{content});

    // 流式读取
    {
        const file = try cwd.openFile("test.txt", .{});
        defer file.close();

        var buf_reader = std.io.bufferedReader(file.reader());
        const reader = buf_reader.reader();

        var line_buf: [1024]u8 = undefined;
        var line_num: usize = 0;
        while (try reader.readUntilDelimiterOrEof(&line_buf, '\n')) |line| {
            line_num += 1;
            std.log.info("Line {}: {s}", .{ line_num, line });
        }
    }

    // 逐字节读取
    {
        const file = try cwd.openFile("test.txt", .{});
        defer file.close();

        const reader = file.reader();
        var byte_count: usize = 0;
        while (true) {
            const byte = reader.readByte() catch break;
            byte_count += 1;
            _ = byte;
        }
        std.log.info("Total bytes: {}", .{byte_count});
    }

    // 内存映射 (如果支持)
    // const mmap = try std.os.mmap(...);
}
