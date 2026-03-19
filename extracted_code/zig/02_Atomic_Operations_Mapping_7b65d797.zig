//
// Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\02_Atomic_Operations_Mapping.md
// Line: 725
// Language: zig
// Block ID: 7b65d797
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig 信号栅栏
var signal_flag = Atomic(i32).init(0);

fn signalHandler(sig: i32) callconv(.C) void {
    _ = sig;
    std.atomic.signalFence(.Release);
    signal_flag.store(1, .Monotonic);
}
