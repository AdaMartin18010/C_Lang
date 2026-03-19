//
// Auto-generated from: Zig\Zig_WebAssembly.md
// Line: 341
// Language: zig
// Block ID: d0f1ec11
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// interop.zig

// 结构体定义
const Point = extern struct {
    x: f64,
    y: f64,
};

const Rect = extern struct {
    top_left: Point,
    bottom_right: Point,
};

// 导出结构体操作
export fn create_point(x: f64, y: f64) Point {
    return .{ .x = x, .y = y };
}

export fn point_distance(a: Point, b: Point) f64 {
    const dx = a.x - b.x;
    const dy = a.y - b.y;
    return std.math.sqrt(dx * dx + dy * dy);
}

export fn rect_area(r: Rect) f64 {
    const width = r.bottom_right.x - r.top_left.x;
    const height = r.bottom_right.y - r.top_left.y;
    return width * height;
}

// 数组操作
export fn sum_array(ptr: [*]const f64, len: usize) f64 {
    var sum: f64 = 0;
    for (0..len) |i| {
        sum += ptr[i];
    }
    return sum;
}
