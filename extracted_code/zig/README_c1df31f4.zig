//
// Auto-generated from: 08_Zig_C_Connection\01_Type_System_Mapping\README.md
// Line: 382
// Language: zig
// Block ID: c1df31f4
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// C 函数指针映射
const c = @cImport(@cInclude("stdlib.h"));

// 定义函数指针类型
const CompareFn = ?*const fn (?*const anyopaque, ?*const anyopaque) callconv(.C) c_int;

pub fn qsortDemo() void {
    var arr = [5]i32{ 3, 1, 4, 1, 5 };

    // 定义比较函数
    const cmp = struct {
        fn compare(a: ?*const anyopaque, b: ?*const anyopaque) callconv(.C) c_int {
            const a_ptr: *const i32 = @ptrCast(@alignCast(a));
            const b_ptr: *const i32 = @ptrCast(@alignCast(b));
            return if (a_ptr.* < b_ptr.*) -1
                   else if (a_ptr.* > b_ptr.*) 1
                   else 0;
        }
    }.compare;

    c.qsort(&arr, arr.len, @sizeOf(i32), cmp);
}
