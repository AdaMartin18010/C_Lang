//
// Auto-generated from: 08_Zig_C_Connection\05_Migration_Methodology\01_Incremental_Migration_Patterns.md
// Line: 2240
// Language: zig
// Block ID: 4f7cea21
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig 模糊测试配置

const std = @import("std");

/// 为 C/Zig 边界生成模糊测试输入
pub fn fuzzTarget(data: []const u8) void {
    // 测试解析器
    _ = ZigParser.parse(data) catch |e| switch (e) {
        error.InvalidInput => return,  // 预期错误
        error.OutOfMemory => return,   // 资源限制
        else => @panic("Unexpected error"),
    };

    // 对比 C 实现行为
    const c_result = CParser.parse(data);
    const zig_result = ZigParser.parse(data);

    // 行为应该一致
    switch (c_result) {
        .ok => if (zig_result != .ok) @panic("Divergence: C ok, Zig error"),
        .error_ => if (zig_result == .ok) @panic("Divergence: C error, Zig ok"),
    }
}

// libFuzzer 集成
export fn LLVMFuzzerTestOneInput(data: [*c]const u8, size: usize) c_int {
    fuzzTarget(data[0..size]);
    return 0;
}
