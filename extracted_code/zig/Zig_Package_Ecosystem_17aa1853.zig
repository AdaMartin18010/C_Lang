//
// Auto-generated from: Zig\Zig_Package_Ecosystem.md
// Line: 413
// Language: zig
// Block ID: 17aa1853
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//


// 公共 API
pub const core = @import("core/utils.zig");
pub const types = @import("core/types.zig");
pub const module_a = @import("modules/module_a.zig");
pub const module_b = @import("modules/module_b.zig");

// 重新导出常用类型
pub const Point = types.Point;
pub const Rectangle = types.Rectangle;

// 版本信息
pub const version = @import("std").SemanticVersion.parse("1.2.3") catch unreachable;
