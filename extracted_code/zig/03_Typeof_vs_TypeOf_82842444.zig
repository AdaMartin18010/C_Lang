//
// Auto-generated from: 08_Zig_C_Connection\06_C23_Zig_Parallel\03_Typeof_vs_TypeOf.md
// Line: 980
// Language: zig
// Block ID: 82842444
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig 复合类型推导

const Point = struct {
    x: i32,
    y: i32,
};

const IntFloat = union {
    i: i32,
    f: f32,
};

const Color = enum {
    red,
    green,
    blue,
};

fn zigCompositeTypeof() void {
    var p = Point{ .x = 1, .y = 2 };
    var uf = IntFloat{ .i = 42 };
    var c = Color.red;

    // 结构体推导
    const PointType = @TypeOf(p);     // Point
    const PxType = @TypeOf(p.x);      // i32

    var p2: PointType = .{ .x = 3, .y = 4 };

    // 联合体推导
    const UfType = @TypeOf(uf);       // IntFloat
    const FiType = @TypeOf(uf.i);     // i32

    // 枚举推导
    const ColorType = @TypeOf(c);     // Color
    const EType = @TypeOf(Color.red); // Color

    _ = .{ p2, UfType, FiType, ColorType, EType };
}

// 匿名结构体推导
fn zigAnonymousStruct() void {
    var anon = struct {
        pt: Point,
        col: Color,
    }{
        .pt = .{ .x = 1, .y = 2 },
        .col = .red,
    };

    const AnonType = @TypeOf(anon);
    const PtType = @TypeOf(anon.pt);
    const ColType = @TypeOf(anon.col);

    _ = .{ AnonType, PtType, ColType };
}
