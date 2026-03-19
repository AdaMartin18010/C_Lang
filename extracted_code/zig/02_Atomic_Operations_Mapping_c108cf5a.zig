//
// Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\02_Atomic_Operations_Mapping.md
// Line: 289
// Language: zig
// Block ID: c108cf5a
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig 原子存储
var value = Atomic(u64).init(0);

fn storeExamples() void {
    value.store(42, .SeqCst);
    value.store(42, .Monotonic);
    value.store(42, .Release);
}
