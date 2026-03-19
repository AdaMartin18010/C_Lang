//
// Auto-generated from: 08_Zig_C_Connection\03_ABI_Formalization\01_System_V_ABI_Zig_C.md
// Line: 1258
// Language: zig
// Block ID: fcdf5dca
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig 代码: 虚函数表兼容实现

const Shape = extern struct {
    vtable: *const VTable,
    data: i32,
};

const VTable = extern struct {
    area: *const fn (*const Shape) callconv(.C) f64,
    draw: *const fn (*const Shape) callconv(.C) void,
    destroy: *const fn (*Shape) callconv(.C) void,
};

// C 兼容的虚函数
export fn circleArea(self: *const Shape) callconv(.C) f64 {
    // 实现...
    return 3.14159 * @as(f64, @floatFromInt(self.data));
}

export fn circleDraw(self: *const Shape) callconv(.C) void {
    _ = self;
    // 实现...
}

export const circleVTable = VTable{
    .area = circleArea,
    .draw = circleDraw,
    .destroy = circleDestroy,
};

export fn circleDestroy(self: *Shape) callconv(.C) void {
    // 清理资源
    _ = self;
}

// 虚函数调用 (与 C 生成的代码完全兼容)
export fn getArea(s: *const Shape) callconv(.C) f64 {
    return s.vtable.area(s);
}
