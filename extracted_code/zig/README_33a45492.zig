//
// Auto-generated from: 08_Zig_C_Connection\03_ABI_Formalization\README.md
// Line: 236
// Language: zig
// Block ID: 33a45492
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig 中的等效声明
const Point3D = extern struct {
    x: f64,
    y: f64,
    z: f64,
};

// 确保与 C 相同的传递方式
extern fn transform_point(p: Point3D, matrix: *[3][3]f64) Point3D;

// 验证结构体大小和布局
comptime {
    @import("std").debug.assert(@sizeOf(Point3D) == 24);
    @import("std").debug.assert(@alignOf(Point3D) == 8);
}
