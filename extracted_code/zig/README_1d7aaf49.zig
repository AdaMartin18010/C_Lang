//
// Auto-generated from: 08_Zig_C_Connection\01_Type_System_Mapping\README.md
// Line: 192
// Language: zig
// Block ID: 1d7aaf49
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig 中使用 C 联合体
const c = @cImport(@cInclude("value.h"));

pub fn unionDemo() void {
    var val: c.Value = undefined;

    // 访问不同成员
    val.i = 42;
    std.debug.print("As int: {d}\n", .{val.i});

    val.f = 3.14;
    std.debug.print("As float: {e}\n", .{val.f});
}
