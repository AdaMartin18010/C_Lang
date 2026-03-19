//
// Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\02_Atomic_Operations_Mapping.md
// Line: 264
// Language: zig
// Block ID: 9e9ea9be
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig 原子加载
var value = Atomic(u64).init(0);

fn loadExamples() void {
    const v1 = value.load(.SeqCst);
    const v2 = value.load(.Monotonic);
    const v3 = value.load(.Acquire);
    _ = v1 + v2 + v3;
}
