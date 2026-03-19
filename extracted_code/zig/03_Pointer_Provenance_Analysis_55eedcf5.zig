//
// Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\03_Pointer_Provenance_Analysis.md
// Line: 871
// Language: zig
// Block ID: 55eedcf5
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig 指针算术与来源
fn zigPointerArithmetic() void {
    var arr: [10]i32 = undefined;
    const base: [*]i32 = &arr;

    // Zig 的多指针算术
    const p1 = base + 5;   // OK
    const p2 = base + 10;  // OK: one-past-end
    const diff = p2 - p1;  // OK: 5

    // 使用切片更安全
    const slice: []i32 = &arr;
    const elem = slice[5];  // 运行时边界检查
    _ = elem;

    _ = diff;
}

// Zig 的 @ptrOffset 和 @ptrSub
fn zigPointerOperations() void {
    var arr: [10]i32 = undefined;
    const ptr: [*]i32 = &arr;

    // 指针加法
    const p2 = ptr + 2;

    // 指针减法
    const diff = p2 - ptr;  // 结果: 2

    _ = diff;
}
