//
// Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\02_Atomic_Operations_Mapping.md
// Line: 685
// Language: zig
// Block ID: 8b2a0287
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig 线程栅栏
fn fenceExamples() void {
    std.atomic.fence(.Acquire);
    std.atomic.fence(.Release);
    std.atomic.fence(.AcqRel);
    std.atomic.fence(.SeqCst);
}

// Release-Acquire 栅栏配对
var data: i32 = 0;
var flag = Atomic(i32).init(0);

fn producerWithFence() void {
    data = 42;
    std.atomic.fence(.Release);
    flag.store(1, .Monotonic);
}

fn consumerWithFence() void {
    while (flag.load(.Monotonic) != 1) {}
    std.atomic.fence(.Acquire);
    const value = data;
    _ = value;
}
