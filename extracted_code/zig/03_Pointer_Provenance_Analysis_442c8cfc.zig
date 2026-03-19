//
// Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\03_Pointer_Provenance_Analysis.md
// Line: 360
// Language: zig
// Block ID: 442c8cfc
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig 指针属性系统
const std = @import("std");

fn zigPointerAttributes() !void {
    // Zig 指针类型显式携带属性

    // [*]T - 多指针，允许索引，可能为 null
    // [*c]T - C 风格指针，用于 FFI
    // [*:sentinel]T - 以哨兵结尾的多指针
    // *T - 单指针，必须非 null
    // ?*T - 可选单指针，可能为 null
    // []T - 切片（胖指针：ptr + len）

    var arr: [10]i32 = undefined;

    // 单指针：编译时知道来源
    const single: *i32 = &arr[0];
    _ = single;

    // 切片：携带长度信息（空间边界）
    const slice: []i32 = &arr;
    _ = slice;

    // 多指针：需谨慎处理边界
    const many: [*]i32 = &arr;
    _ = many;

    // 可选指针：显式处理 null
    const maybe: ?*i32 = if (arr.len > 0) &arr[0] else null;
    if (maybe) |ptr| {
        _ = ptr;
    }
}

// 指针属性组合
test "pointer provenance attributes" {
    const arr = [_]i32{ 1, 2, 3, 4, 5 };

    // const 指针：不可变来源
    const const_ptr: *const i32 = &arr[0];
    _ = const_ptr;

    // align 属性：对齐来源
    const aligned: *align(64) i32 = @ptrCast(@alignCast(&arr[0]));
    _ = aligned;

    // volatile 属性：阻止优化
    const vol: *volatile i32 = @ptrCast(&arr[0]);
    _ = vol;
}
