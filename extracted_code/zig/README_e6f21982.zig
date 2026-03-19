//
// Auto-generated from: 08_Zig_C_Connection\01_Type_System_Mapping\README.md
// Line: 339
// Language: zig
// Block ID: e6f21982
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig 中导入和使用
const c = @cImport(@cInclude("math_ops.h"));

// 直接调用 C 函数
pub fn callCFunctions() void {
    const sum = c.add(5, 3);
    const result = c.compute(10, 2.5);

    // 传递 Zig 函数作为 C 回调
    const S = struct {
        pub fn myCallback(x: c_int) callconv(.C) c_int {
            return x * 2;
        }
    };

    c.process_data("test", 4, S.myCallback);
}
