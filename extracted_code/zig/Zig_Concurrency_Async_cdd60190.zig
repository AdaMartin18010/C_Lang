//
// Auto-generated from: Zig\Zig_Concurrency_Async.md
// Line: 147
// Language: zig
// Block ID: cdd60190
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 线程栈大小配置
const thread = try std.Thread.spawn(.{
    .stack_size = 256 * 1024,  // 256KB 栈
}, worker, .{});

// 线程局部存储
threadlocal var thread_id: usize = 0;

fn setThreadId(id: usize) void {
    thread_id = id;
}

// 线程池配置
var pool: std.Thread.Pool = undefined;
try std.Thread.Pool.init(&pool, .{
    .allocator = allocator,
    .n_jobs = 4,  // 4 个工作线程
});
defer pool.deinit();

// 提交任务到线程池
try pool.spawn(workerTask, .{});
