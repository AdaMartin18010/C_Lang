//
// Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\02_Atomic_Operations_Mapping.md
// Line: 391
// Language: zig
// Block ID: 928c1805
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig Fetch-Op 操作
var counter = Atomic(i32).init(0);
var bitmap = Atomic(u64).init(0);

fn fetchOpExamples() void {
    // 算术操作
    const old1 = counter.fetchAdd(1, .SeqCst);
    const old2 = counter.fetchSub(1, .SeqCst);

    // 位操作
    const old3 = bitmap.fetchOr(0xFF, .SeqCst);
    const old4 = bitmap.fetchAnd(~@as(u64, 0xFF), .SeqCst);
    const old5 = bitmap.fetchXor(0x0F, .SeqCst);
    _ = old1; _ = old2; _ = old3; _ = old4; _ = old5;
}
