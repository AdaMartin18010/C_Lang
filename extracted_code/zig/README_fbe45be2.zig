//
// Auto-generated from: 08_Zig_C_Connection\01_Type_System_Mapping\README.md
// Line: 158
// Language: zig
// Block ID: fbe45be2
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig 中使用 C 结构体
const c = @cImport(@cInclude("user.h"));

pub fn main() void {
    // 直接创建 C 结构体实例
    var user: c.User = .{
        .id = 12345,
        .name = undefined,  // 数组需要单独初始化
        .age = 25,
    };

    // 复制字符串到数组
    const name = "Alice";
    @memcpy(&user.name, name.ptr, name.len);
    user.name[name.len] = 0;  // 空终止

    // 调用 C 函数
    var new_user = c.create_user(67890, "Bob", 30);
}
