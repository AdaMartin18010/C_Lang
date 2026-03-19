//
// Auto-generated from: Zig\Zig_OS_Development.md
// Line: 634
// Language: zig
// Block ID: 8cf6ba6a
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// scheduler.zig - 进程调度器
const std = @import("std");

pub const ProcessState = enum {
    ready,
    running,
    blocked,
    zombie,
};

pub const Process = struct {
    pid: usize,
    state: ProcessState,
    context: Context,
    priority: u8,
    time_slice: usize,
};

pub const RoundRobinScheduler = struct {
    ready_queue: std.ArrayList(*Process),
    current: ?*Process,

    pub fn schedule(self: *RoundRobinScheduler) ?*Process {
        if (self.ready_queue.items.len == 0) return null;

        const next = self.ready_queue.orderedRemove(0);
        next.state = .running;

        if (self.current) |current| {
            if (current.state == .running) {
                current.state = .ready;
                self.ready_queue.append(current) catch {};
            }
        }

        self.current = next;
        return next;
    }
};
