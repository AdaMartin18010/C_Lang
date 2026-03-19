//
// Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\03_Pointer_Provenance_Analysis.md
// Line: 558
// Language: zig
// Block ID: 68bcf41a
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig 的别名规则
// Zig 默认采用与 C 类似的严格别名规则，但提供更安全的类型转换

fn zigAliasing() void {
    var i: i32 = 42;
    var f: f32 = 3.14;

    // Zig 防止隐式类型双关
    // const fp: *f32 = &i;  // 编译错误：类型不匹配

    // 必须显式使用 @ptrCast
    const fp: *f32 = @ptrCast(&i);
    // 但 @ptrCast 是 unsafe 操作，需要知道风险

    // 安全的方式：使用 union
    const IntFloat = extern union {
        i: i32,
        f: f32,
    };

    var u: IntFloat = .{ .i = 42 };
    const val = u.f;  // 安全
    _ = val;
    _ = fp;
}

// @ptrCast 的使用场景和注意事项
fn ptrCastExample() void {
    var bytes: [4]u8 = .{ 0x00, 0x00, 0x80, 0x41 };  // 16.0f 的表示

    // 将字节数组解释为 f32
    const f: *f32 = @ptrCast(&bytes);
    // 注意：这违反严格别名规则！

    // 更好的方式：使用 std.mem.bytesToValue
    const value = std.mem.bytesToValue(f32, &bytes);
    std.debug.assert(value == 16.0);
    _ = f;
}
