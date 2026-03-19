//
// Auto-generated from: 08_Zig_C_Connection\01_Type_System_Mapping\README.md
// Line: 119
// Language: zig
// Block ID: bd9246b0
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 布尔值转换
const c = @cImport(@cInclude("stdbool.h"));

pub fn boolDemo() void {
    var c_bool: bool = true;  // C bool
    var zig_bool: bool = c_bool;  // 直接赋值

    // C 风格 0/1 整数转换为 bool
    var int_val: c_int = 1;
    var bool_from_int: bool = int_val != 0;
}
