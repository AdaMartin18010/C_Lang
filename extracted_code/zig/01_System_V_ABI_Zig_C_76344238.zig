//
// Auto-generated from: 08_Zig_C_Connection\03_ABI_Formalization\01_System_V_ABI_Zig_C.md
// Line: 1055
// Language: zig
// Block ID: 76344238
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig 代码: 栈帧示例

export fn stackFrameExample(x: i32, y: i32) void {
    const local1 = x + y;
    const local2 = x - y;

    const stdout = std.io.getStdOut().writer();
    stdout.print("{d} {d}\n", .{ local1, local2 }) catch {};
}

// Zig 可能生成类似的代码
// 优化时可能内联 printf 或使用不同的输出方式
