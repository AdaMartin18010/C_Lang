//
// Auto-generated from: Zig\Zig_Testing_Debugging.md
// Line: 139
// Language: zig
// Block ID: 9d28eb09
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

const testing = std.testing;

// 相等性测试
try testing.expectEqual(expected, actual);
try testing.expectEqualStrings("hello", actual_string);
try testing.expectEqualSlices(u8, &expected_array, actual_slice);

// 布尔测试
try testing.expect(condition);
try testing.expect(!condition);

// 错误测试
try testing.expectError(expected_error, actual_result);

// 指针测试
try testing.expectEqualPtr(expected_ptr, actual_ptr);
try testing.expect(null_pointer == null);

// 浮点数测试 (允许误差)
try testing.expectApproxEqAbs(expected: f64, actual: f64, tolerance: f64);
try testing.expectApproxEqRel(expected: f64, actual: f64, tolerance: f64);

// 自定义错误信息
try testing.expectEqual(@as(i32, 5), add(2, 3));
