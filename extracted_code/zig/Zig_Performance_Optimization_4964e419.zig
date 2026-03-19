//
// Auto-generated from: Zig\Zig_Performance_Optimization.md
// Line: 612
// Language: zig
// Block ID: 4964e419
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 避免伪共享 (False Sharing)

// ❌ 多个线程修改相邻数据
const BadCounter = struct {
    counters: [8]u64,  // 在同一个缓存行
};

// ✅ 填充到独立缓存行
const CACHE_LINE = 64;
const GoodCounter = struct {
    counters: [8]align(CACHE_LINE) CacheAlignedCounter,
};

const CacheAlignedCounter = extern struct {
    value: u64,
    padding: [CACHE_LINE - @sizeOf(u64)]u8,
};

// 使用
pub fn incrementCounters(counters: *GoodCounter) void {
    var threads: [8]std.Thread = undefined;

    for (&threads, 0..) |*t, i| {
        t.* = std.Thread.spawn(.{}, struct {
            fn run(idx: usize, c: *GoodCounter) void {
                for (0..1_000_000) |_| {
                    _ = @atomicRmw(u64, &c.counters[idx].value, .Add, 1, .Monotonic);
                }
            }
        }.run, .{ i, counters }) catch unreachable;
    }

    for (threads) |t| t.join();
}
