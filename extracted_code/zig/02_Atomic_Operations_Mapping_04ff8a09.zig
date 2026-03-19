//
// Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\02_Atomic_Operations_Mapping.md
// Line: 316
// Language: zig
// Block ID: 04ff8a09
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig 原子交换
var lock = Atomic(bool).init(false);

fn tryLock() bool {
    return lock.swap(true, .Acquire);
}

fn unlock() void {
    lock.store(false, .Release);
}
