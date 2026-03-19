//
// Auto-generated from: Zig\Zig_Standard_Library_Deep_Dive.md
// Line: 1265
// Language: zig
// Block ID: 2853cb50
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

fn process_management() !void {
    const allocator = std.heap.page_allocator;

    // 执行子进程
    const result = try std.process.Child.run(.{
        .allocator = allocator,
        .argv = &.{ "ls", "-la" },
    });
    defer {
        allocator.free(result.stdout);
        allocator.free(result.stderr);
    }

    std.log.info("Exit code: {}", .{result.term.Exited});
    std.log.info("Output:\n{s}", .{result.stdout});

    // 管道通信
    const child = try std.process.Child.init(
        &.{ "grep", "zig" },
        allocator
    );

    child.stdin_behavior = .Pipe;
    child.stdout_behavior = .Pipe;

    try child.spawn();

    // 向子进程写入
    const input = "zig is great\nother line\nzig lang\n";
    _ = try child.stdin.?.write(input);
    child.stdin.?.close();
    child.stdin = null;

    // 读取输出
    var buf: [1024]u8 = undefined;
    const n = try child.stdout.?.read(&buf);
    std.log.info("Filtered:\n{s}", .{buf[0..n]});

    _ = try child.wait();
}

fn thread_management() !void {
    // 见前文异步章节
    const t1 = try std.Thread.spawn(.{}, worker_fn, .{1});
    const t2 = try std.Thread.spawn(.{}, worker_fn, .{2});

    t1.join();
    t2.join();
}

fn worker_fn(id: usize) void {
    std.log.info("Worker {} started", .{id});
    std.time.sleep(100 * std.time.ns_per_ms);
    std.log.info("Worker {} done", .{id});
}
