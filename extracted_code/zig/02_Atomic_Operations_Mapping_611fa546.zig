//
// Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\02_Atomic_Operations_Mapping.md
// Line: 631
// Language: zig
// Block ID: 611fa546
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig 原子指针
var atomic_int_ptr = Atomic(?*i32).init(null);

fn ptrExample() void {
    var value: i32 = 42;

    // 存储
    atomic_int_ptr.store(&value, .Release);

    // 加载
    const ptr = atomic_int_ptr.load(.Acquire);

    // 交换
    var value2: i32 = 100;
    const old = atomic_int_ptr.swap(&value2, .AcqRel);
    _ = old;
}
