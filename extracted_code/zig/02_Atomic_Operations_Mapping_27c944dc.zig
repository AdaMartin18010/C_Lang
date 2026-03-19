//
// Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\02_Atomic_Operations_Mapping.md
// Line: 355
// Language: zig
// Block ID: 27c944dc
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig 比较并交换
var counter = Atomic(i32).init(0);

fn casExample(expected: i32, desired: i32) bool {
    // 方式 1: cmpxchg 返回结构体
    const result = counter.cmpxchg(expected, desired, .SeqCst, .SeqCst);
    return result.success;
}

fn casExample2(expected: i32, desired: i32) bool {
    // 方式 2: tryCmpxchg 返回可选值
    const result = counter.tryCmpxchg(expected, desired, .AcqRel, .Monotonic);
    return result == null;  // null 表示成功
}
