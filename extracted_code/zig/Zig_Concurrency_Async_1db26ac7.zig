//
// Auto-generated from: Zig\Zig_Concurrency_Async.md
// Line: 262
// Language: zig
// Block ID: 1db26ac7
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 条件变量用于线程间等待/通知
var ready = false;
var condition = std.Thread.Condition{};
var cond_mutex = std.Thread.Mutex{};

// 等待线程
fn waiter() void {
    cond_mutex.lock();
    defer cond_mutex.unlock();

    while (!ready) {
        condition.wait(&cond_mutex);
    }

    std.log.info("Got signal!", .{});
}

// 通知线程
fn notifier() void {
    std.time.sleep(1 * std.time.ns_per_s);

    cond_mutex.lock();
    defer cond_mutex.unlock();

    ready = true;
    condition.signal();  // 通知一个等待线程
    // condition.broadcast();  // 通知所有等待线程
}
