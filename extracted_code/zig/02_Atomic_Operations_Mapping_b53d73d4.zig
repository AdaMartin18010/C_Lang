//
// Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\02_Atomic_Operations_Mapping.md
// Line: 514
// Language: zig
// Block ID: b53d73d4
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig Sequential Consistency
var x = Atomic(i32).init(0);
var y = Atomic(i32).init(0);

fn thread1() void {
    x.store(1, .SeqCst);
    const r1 = y.load(.SeqCst);
    _ = r1;
}

fn thread2() void {
    y.store(1, .SeqCst);
    const r2 = x.load(.SeqCst);
    _ = r2;
}
