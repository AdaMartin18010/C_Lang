//
// Auto-generated from: Zig\Zig_Package_Ecosystem.md
// Line: 585
// Language: zig
// Block ID: f01231a1
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 版本兼容性工具

const std = @import("std");

pub const Version = struct {
    major: u32,
    minor: u32,
    patch: u32,

    pub fn parse(str: []const u8) !Version {
        var parts = std.mem.split(u8, str, ".");

        const major = try std.fmt.parseInt(u32, parts.next() orelse return error.InvalidVersion, 10);
        const minor = try std.fmt.parseInt(u32, parts.next() orelse return error.InvalidVersion, 10);
        const patch = try std.fmt.parseInt(u32, parts.next() orelse return error.InvalidVersion, 10);

        return .{ .major = major, .minor = minor, .patch = patch };
    }

    // 检查版本兼容性
    pub fn isCompatible(self: Version, required: Version) bool {
        // 主版本必须相同
        if (self.major != required.major) return false;

        // 次要版本必须 >= 要求
        if (self.minor < required.minor) return false;

        // 如果是相同次要版本，补丁必须 >= 要求
        if (self.minor == required.minor and self.patch < required.patch) {
            return false;
        }

        return true;
    }

    pub fn format(
        self: Version,
        comptime fmt: []const u8,
        options: std.fmt.FormatOptions,
        writer: anytype,
    ) !void {
        _ = fmt;
        _ = options;
        try writer.print("{}.{}.{}", .{ self.major, self.minor, self.patch });
    }
};

// 依赖版本约束
pub const VersionConstraint = struct {
    min: ?Version,
    max: ?Version,
    exact: ?Version,

    pub fn matches(self: VersionConstraint, version: Version) bool {
        if (self.exact) |exact| {
            return version.major == exact.major and
                   version.minor == exact.minor and
                   version.patch == exact.patch;
        }

        if (self.min) |min| {
            if (version.major < min.major) return false;
            if (version.major == min.major and version.minor < min.minor) return false;
            if (version.major == min.major and version.minor == min.minor and version.patch < min.patch) {
                return false;
            }
        }

        if (self.max) |max| {
            if (version.major > max.major) return false;
            if (version.major == max.major and version.minor > max.minor) return false;
            if (version.major == max.major and version.minor == max.minor and version.patch > max.patch) {
                return false;
            }
        }

        return true;
    }

    pub fn parse(str: []const u8) !VersionConstraint {
        // 解析版本约束字符串
        // ^1.2.3 - 兼容 1.x.x
        // ~1.2.3 - 兼容 1.2.x
        // >=1.2.3 - 大于等于
        // 1.2.3 - 精确版本

        if (str[0] == '^') {
            const min = try Version.parse(str[1..]);
            return .{ .min = min, .max = .{ .major = min.major + 1, .minor = 0, .patch = 0 }, .exact = null };
        }

        if (str[0] == '~') {
            const min = try Version.parse(str[1..]);
            return .{ .min = min, .max = .{ .major = min.major, .minor = min.minor + 1, .patch = 0 }, .exact = null };
        }

        const exact = try Version.parse(str);
        return .{ .min = null, .max = null, .exact = exact };
    }
};
