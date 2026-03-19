//
// Auto-generated from: Zig\Zig_C_Interoperability.md
// Line: 385
// Language: zig
// Block ID: d60b6116
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// zig_lib.zig - 导出给 C 使用的库

const std = @import("std");

// 导出函数
export fn zig_add(a: c_int, b: c_int) c_int {
    return a + b;
}

export fn zig_process_data(data: [*c]const u8, len: usize) c_int {
    const slice = data[0..len];
    // 处理数据...
    _ = slice;
    return 0;
}

// 导出全局变量
export var zig_global_counter: c_int = 0;

// 导出类型
export const ZigPoint = extern struct {
    x: f64,
    y: f64,
};

export fn zig_point_distance(a: *const ZigPoint, b: *const ZigPoint) f64 {
    const dx = a.x - b.x;
    const dy = a.y - b.y;
    return std.math.sqrt(dx * dx + dy * dy);
}
