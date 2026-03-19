//
// Auto-generated from: Zig\Zig_Testing_Debugging.md
// Line: 435
// Language: zig
// Block ID: 9e80452d
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// benchmark.zig

const std = @import("std");
const testing = std.testing;

fn benchmark(name: []const u8, func: fn () void, iterations: usize) !void {
    const start = std.time.nanoTimestamp();

    var i: usize = 0;
    while (i < iterations) : (i += 1) {
        func();
    }

    const end = std.time.nanoTimestamp();
    const elapsed = @as(f64, @floatFromInt(end - start)) / std.time.ns_per_ms;

    std.debug.print("{s}: {d:.2} ms for {} iterations\n", .{
        name, elapsed, iterations
    });
}

fn functionToBenchmark() void {
    var sum: u64 = 0;
    var i: u32 = 0;
    while (i < 10000) : (i += 1) {
        sum += i;
    }
    std.mem.doNotOptimizeAway(sum);
}

test "benchmark example" {
    try benchmark("functionToBenchmark", functionToBenchmark, 1000);
}
