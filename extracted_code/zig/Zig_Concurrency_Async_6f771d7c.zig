//
// Auto-generated from: Zig\Zig_Concurrency_Async.md
// Line: 216
// Language: zig
// Block ID: 6f771d7c
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

const std = @import("std");

// 基本 Mutex
var counter: i32 = 0;
var mutex = std.Thread.Mutex{};

fn incrementCounter() void {
    mutex.lock();
    defer mutex.unlock();

    counter += 1;
}

// 作用域锁 (避免忘记解锁)
fn scopedLock() void {
    {
        mutex.lock();
        defer mutex.unlock();

        // 临界区
        counter += 1;
    }  // 自动解锁
}

// 读写锁
var data: [100]i32 = undefined;
var rwlock = std.Thread.RwLock{};

fn readData(index: usize) i32 {
    rwlock.lockShared();
    defer rwlock.unlockShared();

    return data[index];
}

fn writeData(index: usize, value: i32) void {
    rwlock.lock();
    defer rwlock.unlock();

    data[index] = value;
}
