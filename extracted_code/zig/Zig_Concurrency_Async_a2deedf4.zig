//
// Auto-generated from: Zig\Zig_Concurrency_Async.md
// Line: 399
// Language: zig
// Block ID: a2deedf4
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 注意: Zig 的 async/await 正在重新设计
// 以下内容基于 Zig 0.11 之前的实验性实现

const std = @import("std");

// 异步函数
async fn fetchData(url: []const u8) ![]u8 {
    // 模拟异步 I/O
    await asyncSleep(100);
    return "data";
}

// 另一个异步函数
async fn process(data: []const u8) !void {
    // 处理数据
    await asyncSleep(50);
}

// 组合异步操作
async fn fetchAndProcess(url: []const u8) !void {
    const data = try await fetchData(url);
    try await process(data);
}

// 并行执行
async fn parallelFetch(urls: []const []const u8) !void {
    var frame1 = async fetchData(urls[0]);
    var frame2 = async fetchData(urls[1]);
    var frame3 = async fetchData(urls[2]);

    const data1 = try await frame1;
    const data2 = try await frame2;
    const data3 = try await frame3;

    _ = data1;
    _ = data2;
    _ = data3;
}

// 带超时的操作
async fn fetchWithTimeout(url: []const u8, timeout_ms: u64) ![]u8 {
    var fetch_frame = async fetchData(url);
    var timer_frame = async asyncSleep(timeout_ms);

    // 等待任一完成
    // 简化示例
    const data = try await fetch_frame;
    return data;
}
