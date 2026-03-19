//
// Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\03_Pointer_Provenance_Analysis.md
// Line: 1291
// Language: zig
// Block ID: 4393bff4
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig 的指针比较规则
fn zigPointerComparison() void {
    var arr: [10]i32 = undefined;

    // 单指针比较
    const p1: *i32 = &arr[0];
    const p2: *i32 = &arr[5];

    const less = p1 < p2;  // 合法比较
    _ = less;

    // 多指针比较
    const mp1: [*]i32 = &arr;
    const mp2: [*]i32 = mp1 + 5;

    const less2 = mp1 < mp2;  // 合法
    _ = less2;

    // 切片比较
    const slice = arr[0..5];
    const slice2 = arr[5..10];

    // 切片指针比较
    const sless = slice.ptr < slice2.ptr;
    _ = sless;
}

// 不同来源指针的比较
fn zigDifferentProvenanceComparison() void {
    var arr1: [10]i32 = undefined;
    var arr2: [10]i32 = undefined;

    const p1 = &arr1[0];
    const p2 = &arr2[0];

    // Zig 允许不同来源的指针比较
    // 但结果可能是实现定义的
    const result = p1 < p2;
    _ = result;

    // 如果指针来自不同分配，比较结果无意义
}

// One-Past-End 在 Zig 中的处理
fn zigOnePastEnd() void {
    var arr: [10]i32 = undefined;
    const base: [*]i32 = &arr;

    const end = base + arr.len;  // one-past-end

    // 遍历模式
    var p = base;
    while (p != end) : (p += 1) {
        p[0] = 42;
    }

    // 更安全：使用切片
    for (&arr) |*elem| {
        elem.* = 42;
    }
}
