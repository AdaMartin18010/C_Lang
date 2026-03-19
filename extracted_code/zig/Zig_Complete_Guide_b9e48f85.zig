//
// Auto-generated from: Zig\Zig_Complete_Guide.md
// Line: 1033
// Language: zig
// Block ID: b9e48f85
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig 的安全模式 (Debug 和 ReleaseSafe 模式)
// 运行时边界检查

fn safety_checks() void {
    var arr = [_]u8{ 1, 2, 3 };

    // 这将触发运行时 panic (索引越界)
    // const x = arr[10];

    // 安全的访问方式
    if (arr.len > 10) {
        const x = arr[10];
        _ = x;
    }

    // 使用 @setRuntimeSafety 控制检查
    @setRuntimeSafety(false);  // 关闭安全检查 (危险！)
    // 现在 arr[10] 不会 panic，但可能导致未定义行为
    @setRuntimeSafety(true);
}

// 释放模式选择
// Debug: 完整安全检查 + 优化关闭
// ReleaseSafe: 完整安全检查 + 优化开启
// ReleaseFast: 最小检查 + 最大优化
// ReleaseSmall: 最小检查 + 最小代码体积
