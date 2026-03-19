//
// Auto-generated from: Zig\Zig_Concurrency_Async.md
// Line: 295
// Language: zig
// Block ID: b28ce82e
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 信号量限制并发数
var semaphore = std.Thread.Semaphore{ .permits = 3 };  // 最多3个并发

fn limitedWorker(id: usize) void {
    semaphore.wait();  // 获取许可
    defer semaphore.post();  // 释放许可

    std.log.info("Worker {} is running", .{id});
    std.time.sleep(500 * std.time.ns_per_ms);
}

// 使用信号量实现生产者-消费者
var items_sem = std.Thread.Semaphore{ .permits = 0 };
var slots_sem = std.Thread.Semaphore{ .permits = 10 };  // 缓冲区大小
var buffer_mutex = std.Thread.Mutex{};
var buffer: [10]i32 = undefined;
var write_idx: usize = 0;
var read_idx: usize = 0;

fn producer() void {
    var i: i32 = 0;
    while (true) : (i += 1) {
        slots_sem.wait();  // 等待空槽

        buffer_mutex.lock();
        buffer[write_idx] = i;
        write_idx = (write_idx + 1) % buffer.len;
        buffer_mutex.unlock();

        items_sem.post();  // 通知有新数据
    }
}

fn consumer() void {
    while (true) {
        items_sem.wait();  // 等待数据

        buffer_mutex.lock();
        const item = buffer[read_idx];
        read_idx = (read_idx + 1) % buffer.len;
        buffer_mutex.unlock();

        slots_sem.post();  // 释放空槽

        std.log.info("Consumed: {}", .{item});
    }
}
