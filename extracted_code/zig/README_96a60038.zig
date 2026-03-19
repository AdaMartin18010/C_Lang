//
// Auto-generated from: 08_Zig_C_Connection\04_Translation_Validation\README.md
// Line: 395
// Language: zig
// Block ID: 96a60038
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

test "fuzz translation equivalence" {
    const c = @cImport(@cInclude("parser.h"));

    // 使用 Zig 的模糊测试框架
    const input_bytes = std.testing.fuzzInput(.{});

    // 两个解析器应该产生相同结果
    const c_result = c.parse(input_bytes.ptr, input_bytes.len);
    const zig_result = zigParse(input_bytes);

    // 验证解析结果等价
    try std.testing.expectEqual(c_result.success, zig_result.success);
    if (c_result.success) {
        try std.testing.expectEqual(c_result.ast_node_count, zig_result.nodes.len);
    }
}
