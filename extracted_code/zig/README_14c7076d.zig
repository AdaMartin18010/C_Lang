//
// Auto-generated from: 08_Zig_C_Connection\01_Type_System_Mapping\README.md
// Line: 276
// Language: zig
// Block ID: 14c7076d
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 数组和切片转换
const c = @cImport(@cInclude("string.h"));

pub fn arrayDemo() void {
    // Zig 数组 (值类型)
    var zig_array: [5]i32 = [1, 2, 3, 4, 5];

    // 数组退化为指针 (C 风格)
    var c_ptr: [*c]i32 = &zig_array;

    // Zig 切片 (胖指针: {ptr, len})
    var slice: []i32 = &zig_array;
    std.debug.print("Length: {d}\n", .{slice.len});

    // 传递给 C 函数
    _ = c.memcmp(slice.ptr, c_ptr, slice.len * @sizeOf(i32));
}
