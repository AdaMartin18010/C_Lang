//
// Auto-generated from: 08_Zig_C_Connection\03_ABI_Formalization\README.md
// Line: 337
// Language: zig
// Block ID: 659f0839
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

test "ABI compatibility verification" {
    const c = @cImport(@cInclude("abi_test.h"));

    // 测试整数寄存器传递
    const int_result = c.test_int_regs(1, 2, 3, 4, 5, 6);
    try std.testing.expectEqual(int_result, 21);

    // 测试浮点寄存器传递
    const float_result = c.test_float_regs(1.0, 2.0, 3.0, 4.0);
    try std.testing.expectApproxEqAbs(float_result, 10.0, 0.001);

    // 测试混合参数
    const mixed_result = c.test_mixed(1, 2.0, 3, 4.0);
    try std.testing.expectEqual(mixed_result, 10.0);
}
