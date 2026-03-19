//
// Auto-generated from: 08_Zig_C_Connection\06_C23_Zig_Parallel\README.md
// Line: 390
// Language: zig
// Block ID: 4121f3bd
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// c_interop.zig - C互操作示例
const std = @import("std");

// 导入C标准库
const c = @cImport({
    @cInclude("stdio.h");
    @cInclude("stdlib.h");
    @cInclude("string.h");
});

// 导出函数供C调用
export fn zig_add(a: i32, b: i32) i32 {
    return a + b;
}

export fn zig_process(data: [*c]const u8, len: usize) i32 {
    const slice = data[0..len];
    _ = slice;
    // 处理数据
    return 0;
}

// 使用C库
pub fn useCStdlib() void {
    const msg = "Hello from Zig\n";
    _ = c.printf("%s", msg);

    const ptr = c.malloc(1024);
    defer c.free(ptr);

    _ = c.memset(ptr, 0, 1024);
}
