//
// Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\02_Atomic_Operations_Mapping.md
// Line: 447
// Language: zig
// Block ID: cdd2e405
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig Monotonic 内存序
var counter = Atomic(u64).init(0);

fn relaxedExample() void {
    _ = counter.fetchAdd(1, .Monotonic);
}
