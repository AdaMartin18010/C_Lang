//
// Auto-generated from: Zig\Zig_OS_Development.md
// Line: 715
// Language: zig
// Block ID: 29cb62bc
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// driver.zig - 驱动框架
const std = @import("std");

pub const Device = struct {
    name: []const u8,
    device_type: DeviceType,
    ops: *const DeviceOps,
    private: ?*anyopaque,

    pub const DeviceType = enum {
        block,
        char,
        network,
        pci,
    };

    pub const DeviceOps = struct {
        init: ?*const fn (*Device) anyerror!void,
        read: ?*const fn (*Device, []u8, usize) anyerror!usize,
        write: ?*const fn (*Device, []const u8, usize) anyerror!usize,
        ioctl: ?*const fn (*Device, u32, usize) anyerror!void,
        close: ?*const fn (*Device) void,
    };
};

pub const DriverManager = struct {
    devices: std.ArrayList(*Device),

    pub fn register(self: *DriverManager, dev: *Device) !void {
        if (dev.ops.init) |init_fn| {
            try init_fn(dev);
        }
        try self.devices.append(dev);
    }
};
