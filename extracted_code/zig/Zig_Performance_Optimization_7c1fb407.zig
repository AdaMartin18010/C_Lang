//
// Auto-generated from: Zig\Zig_Performance_Optimization.md
// Line: 154
// Language: zig
// Block ID: 7c1fb407
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 使用 CPU 性能计数器 (Linux)
const std = @import("std");

pub const PerfCounter = struct {
    fd: i32,

    pub fn init(config: PerfConfig) !PerfCounter {
        const PERF_TYPE_HARDWARE = 0;
        const PERF_COUNT_HW_CPU_CYCLES = 0;
        const PERF_COUNT_HW_INSTRUCTIONS = 1;
        const PERF_COUNT_HW_CACHE_REFERENCES = 2;
        const PERF_COUNT_HW_CACHE_MISSES = 3;

        var attr: std.os.linux.perf_event_attr = .{
            .type = PERF_TYPE_HARDWARE,
            .config = switch (config) {
                .cycles => PERF_COUNT_HW_CPU_CYCLES,
                .instructions => PERF_COUNT_HW_INSTRUCTIONS,
                .cache_refs => PERF_COUNT_HW_CACHE_REFERENCES,
                .cache_misses => PERF_COUNT_HW_CACHE_MISSES,
            },
            .size = @sizeOf(std.os.linux.perf_event_attr),
            .flags = .{},
        };

        const fd = std.os.linux.perf_event_open(&attr, 0, -1, -1, 0);
        if (fd < 0) return error.PerfEventOpenFailed;

        return .{ .fd = fd };
    }

    pub fn deinit(self: PerfCounter) void {
        std.os.close(self.fd);
    }

    pub fn read(self: PerfCounter) u64 {
        var value: u64 = 0;
        _ = std.os.read(self.fd, std.mem.asBytes(&value)) catch return 0;
        return value;
    }

    pub fn reset(self: PerfCounter) void {
        const PERF_EVENT_IOC_RESET = 3;
        _ = std.os.linux.ioctl(self.fd, PERF_EVENT_IOC_RESET, 0);
    }
};

pub const PerfConfig = enum {
    cycles,
    instructions,
    cache_refs,
    cache_misses,
};
