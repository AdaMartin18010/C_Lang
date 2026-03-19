//
// Auto-generated from: 08_Zig_C_Connection\05_Migration_Methodology\01_Incremental_Migration_Patterns.md
// Line: 224
// Language: zig
// Block ID: 5f0dccb2
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig 代码 - 路由层实现
// file: router.zig

const std = @import("std");
const c = @cImport({
    @cInclude("legacy_system.h");
});

// 迁移状态跟踪
const MigrationStatus = enum {
    c_impl,      // 使用 C 实现
    zig_impl,    // 使用 Zig 实现
    dual_run,    // 双运行模式（验证中）
};

// 配置：哪些模块已迁移
const Config = struct {
    auth: MigrationStatus = .c_impl,
    validation: MigrationStatus = .c_impl,
    business_logic: MigrationStatus = .c_impl,
    persistence: MigrationStatus = .c_impl,
};

var config = Config{};

// 用户认证路由
pub fn authenticateUser(username: []const u8, password: []const u8) !bool {
    switch (config.auth) {
        .c_impl => {
            // 调用 C 实现
            const result = c.authenticate_user(
                username.ptr,
                password.ptr
            );
            return result == 1;
        },
        .zig_impl => {
            // 调用 Zig 实现
            return zigAuth.authenticate(username, password);
        },
        .dual_run => {
            // 双运行：比较结果
            const c_result = c.authenticate_user(
                username.ptr,
                password.ptr
            ) == 1;
            const zig_result = zigAuth.authenticate(username, password) catch false;

            if (c_result != zig_result) {
                std.log.warn("Auth divergence detected! C={}, Zig={}",
                    .{c_result, zig_result});
            }
            return zig_result; // 以 Zig 实现为准
        },
    }
}
