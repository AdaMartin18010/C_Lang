//
// Auto-generated from: Zig\Zig_OS_Development.md
// Line: 805
// Language: zig
// Block ID: 38ab98e8
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// vfs.zig - 虚拟文件系统
const std = @import("std");

pub const VNode = struct {
    v_type: VType,
    v_ops: *const VNodeOps,
    v_data: ?*anyopaque,
    v_size: usize,
};

pub const VType = enum {
    regular,
    directory,
    symlink,
    char,
    block,
};

pub const VNodeOps = struct {
    lookup: *const fn (*VNode, []const u8) ?*VNode,
    read: *const fn (*VNode, []u8, usize) anyerror!usize,
    write: *const fn (*VNode, []const u8, usize) anyerror!usize,
    create: *const fn (*VNode, []const u8) anyerror!*VNode,
    mkdir: *const fn (*VNode, []const u8) anyerror!void,
};

pub const VFS = struct {
    root: *VNode,
    mount_points: std.ArrayList(MountPoint),

    pub fn lookup(self: *VFS, path: []const u8) ?*VNode {
        var current = self.root;
        var it = std.mem.tokenizeScalar(u8, path, '/');

        while (it.next()) |component| {
            current = current.v_ops.lookup(current, component) orelse return null;
        }

        return current;
    }
};
