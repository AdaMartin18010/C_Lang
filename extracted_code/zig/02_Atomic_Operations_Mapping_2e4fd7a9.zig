//
// Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\02_Atomic_Operations_Mapping.md
// Line: 474
// Language: zig
// Block ID: 2e4fd7a9
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig Acquire-Release
var data: i32 = 0;
var flag = Atomic(bool).init(false);

fn producer() void {
    data = 42;
    flag.store(true, .Release);
}

fn consumer() void {
    while (!flag.load(.Acquire)) {
        std.atomic.spinLoopHint();
    }
    const value = data;  // 保证看到 42
    _ = value;
}
