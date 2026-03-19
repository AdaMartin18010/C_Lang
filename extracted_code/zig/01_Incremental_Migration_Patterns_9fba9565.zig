//
// Auto-generated from: 08_Zig_C_Connection\05_Migration_Methodology\01_Incremental_Migration_Patterns.md
// Line: 2118
// Language: zig
// Block ID: 9fba9565
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 功能标志模式 - 支持运行时回滚

const std = @import("std");

/// 功能标志配置
pub const FeatureFlags = struct {
    /// 使用 Zig 实现的解析器
    use_zig_parser: bool = true,
    /// 使用 Zig 实现的网络层
    use_zig_network: bool = false,
    /// 使用 Zig 实现的存储层
    use_zig_storage: bool = false,
};

var g_flags = FeatureFlags{};

/// 运行时切换实现
pub fn parseConfig(data: []const u8) !Config {
    if (g_flags.use_zig_parser) {
        return zigParseConfig(data) catch |e| {
            std.log.warn("Zig parser failed: {}, falling back to C", .{e});
            g_flags.use_zig_parser = false;
            return cParseConfig(data);
        };
    } else {
        return cParseConfig(data);
    }
}

/// 渐进式启用功能
pub fn enableFeatureGradually(
    feature: *bool,
    rollout_percentage: f32,
) void {
    const rand = std.crypto.random.float(f32);
    feature.* = rand < rollout_percentage;
}

// 编译时回滚 - 通过条件编译
pub const Parser = if (@import("builtin").mode == .Debug)
    ZigParser  // 调试模式使用 Zig 实现
else
    CParser;   // 发布模式保守使用 C 实现
