//
// Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\02_Atomic_Operations_Mapping.md
// Line: 820
// Language: zig
// Block ID: 1cfd0e54
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig 高效自旋锁
const Spinlock = struct {
    locked: Atomic(bool),

    pub fn init() Spinlock {
        return .{ .locked = Atomic(bool).init(false) };
    }

    pub fn lock(self: *Spinlock) void {
        while (true) {
            if (!self.locked.load(.Monotonic)) {
                if (self.locked.tryCmpxchg(false, true, .Acquire, .Monotonic) == null) {
                    return;
                }
            }
            std.atomic.spinLoopHint();
        }
    }

    pub fn unlock(self: *Spinlock) void {
        self.locked.store(false, .Release);
    }
};
