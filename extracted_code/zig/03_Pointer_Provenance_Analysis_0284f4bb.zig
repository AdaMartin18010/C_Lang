//
// Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\03_Pointer_Provenance_Analysis.md
// Line: 632
// Language: zig
// Block ID: 0284f4bb
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig 的安全防护措施
test "Zig alias protection" {
    var x: i32 = 0;

    // Zig 的 @ptrCast 需要显式，且编译器会警告
    const ip: *i32 = &x;
    const fp: *f32 = @ptrCast(ip);

    // 即使转换了，编译器可能仍进行 TBAA 优化
    ip.* = 1;
    fp.* = 2.0;

    // 结果不确定！
    const result = ip.*;
    _ = result;
}

// 推荐：使用 std.mem 的安全转换
fn safeTypeConversion() void {
    var bytes: [4]u8 = .{ 0x00, 0x00, 0x80, 0x41 };

    // 方式 1：字节复制（安全）
    var f: f32 = undefined;
    @memcpy(std.mem.asBytes(&f), &bytes);

    // 方式 2：使用 bytesToValue
    const f2 = std.mem.bytesToValue(f32, &bytes);

    _ = f;
    _ = f2;
}
