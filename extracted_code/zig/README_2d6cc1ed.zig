//
// Auto-generated from: 08_Zig_C_Connection\05_Migration_Methodology\README.md
// Line: 111
// Language: zig
// Block ID: 2d6cc1ed
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

/// ffi/c_bindings.zig - C 代码的 Zig 绑定层
const std = @import("std");

/// C 库的外部声明集中管理
pub const c = @cImport({
    @cDefine("LEGACY_API_EXPORT", "");
    @cInclude("legacy/core.h");
    @cInclude("legacy/utils.h");
});

/// 类型别名确保一致性
pub const CLegacyContext = c.LegacyContext;
pub const CLegacyStatus = c.LegacyStatus;

/// 错误码映射
pub const LegacyError = error{
    InvalidArgument,
    OutOfMemory,
    IoError,
    Unknown,
};

/// 将 C 错误码转换为 Zig 错误
pub fn statusToError(status: CLegacyStatus) LegacyError!void {
    return switch (status) {
        c.STATUS_OK => {},
        c.STATUS_INVALID_ARG => LegacyError.InvalidArgument,
        c.STATUS_NO_MEM => LegacyError.OutOfMemory,
        c.STATUS_IO_ERROR => LegacyError.IoError,
        else => LegacyError.Unknown,
    };
}
