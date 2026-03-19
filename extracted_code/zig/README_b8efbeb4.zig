//
// Auto-generated from: 08_Zig_C_Connection\01_Type_System_Mapping\README.md
// Line: 100
// Language: zig
// Block ID: b8efbeb4
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 浮点数互操作
const c = @cImport(@cInclude("math.h"));

pub fn calculate() f64 {
    var x: f64 = 3.14159;
    var result = c.sin(x);  // 直接调用 C 数学函数
    return result;
}
