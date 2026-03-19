//
// Auto-generated from: 08_Zig_C_Connection\05_Migration_Methodology\README.md
// Line: 490
// Language: zig
// Block ID: 8ad3be35
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

/// 金丝雀模式 - 部分流量使用新实现
const std = @import("std");

pub const CanaryDeployment = struct {
    zig_impl_ratio: f32,  // 0.0 - 1.0
    random: std.Random,

    pub fn processRequest(self: *CanaryDeployment, req: Request) !Response {
        const use_zig = self.random.float(f32) < self.zig_impl_ratio;

        if (use_zig) {
            return zigProcess(req) catch |err| {
                // 失败时回退到 C 实现
                std.log.warn("Zig impl failed: {s}, falling back to C", .{@errorName(err)});
                return cProcess(req);
            };
        } else {
            return cProcess(req);
        }
    }

    pub fn increaseRatio(self: *CanaryDeployment, delta: f32) void {
        self.zig_impl_ratio = @min(1.0, self.zig_impl_ratio + delta);
    }
};
