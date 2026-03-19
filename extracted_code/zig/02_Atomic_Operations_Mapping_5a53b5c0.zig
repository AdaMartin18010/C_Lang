//
// Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\02_Atomic_Operations_Mapping.md
// Line: 1002
// Language: zig
// Block ID: 5a53b5c0
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig 自适应自旋
fn adaptiveSpin(iteration: u32) void {
    if (iteration < 16) {
        std.atomic.spinLoopHint();
    } else if (iteration < 32) {
        for (0..4) |_| std.atomic.spinLoopHint();
    } else {
        std.Thread.yield() catch {};
    }
}
