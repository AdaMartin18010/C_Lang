//
// Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\07_Zig_C_Interoperability.md
// Line: 336
// Language: zig
// Block ID: 385c0891
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig代码
const c = @cImport({
    @cInclude("point.h");
});

pub fn main() void {
    // 创建C结构体
    const p1 = c.Point{ .x = 1.0, .y = 2.0 };
    const p2 = c.Point{ .x = 3.0, .y = 4.0 };

    // 调用C函数
    const sum = c.point_add(p1, p2);
    const dist = c.point_distance(p1, p2);

    std.debug.print("Sum: ({d}, {d})\n", .{sum.x, sum.y});
    std.debug.print("Distance: {d}\n", .{dist});
}
