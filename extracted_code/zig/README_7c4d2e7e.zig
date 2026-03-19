//
// Auto-generated from: 08_Zig_C_Connection\01_Type_System_Mapping\README.md
// Line: 368
// Language: zig
// Block ID: 7c4d2e7e
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 显式指定调用约定
pub extern "C" fn myCFunction(x: i32) i32 {
    return x + 1;
}

// 或使用 callconv
pub fn myFunction(x: i32) callconv(.C) i32 {
    return x + 1;
}
