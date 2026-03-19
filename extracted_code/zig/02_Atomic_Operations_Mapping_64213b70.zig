//
// Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\02_Atomic_Operations_Mapping.md
// Line: 567
// Language: zig
// Block ID: 64213b70
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig atomic_flag 等效实现
const Atomic = std.atomic.Value;

pub const AtomicFlag = struct {
    flag: Atomic(bool),

    pub const INIT: AtomicFlag = .{ .flag = Atomic(bool).init(false) };

    pub fn testAndSet(self: *AtomicFlag, comptime order: std.atomic.Order) bool {
        return self.flag.swap(true, order);
    }

    pub fn clear(self: *AtomicFlag, comptime order: std.atomic.Order) void {
        self.flag.store(false, order);
    }
};

// 自旋锁
pub const Spinlock = struct {
    locked: Atomic(bool),

    pub fn init() Spinlock {
        return .{ .locked = Atomic(bool).init(false) };
    }

    pub fn lock(self: *Spinlock) void {
        while (self.locked.swap(true, .Acquire)) {
            std.atomic.spinLoopHint();
        }
    }

    pub fn unlock(self: *Spinlock) void {
        self.locked.store(false, .Release);
    }
};
