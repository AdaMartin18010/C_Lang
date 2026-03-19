//
// Auto-generated from: Zig\Zig_OS_Development.md
// Line: 755
// Language: zig
// Block ID: 6de7a560
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// pci.zig - PCI 设备枚举
const std = @import("std");

const PCI_CONFIG_ADDR = 0xCF8;
const PCI_CONFIG_DATA = 0xCFC;

pub fn readConfig(bus: u8, slot: u8, func: u8, offset: u8) u32 {
    const address = @as(u32, 1) << 31 |
        @as(u32, bus) << 16 |
        @as(u32, slot) << 11 |
        @as(u32, func) << 8 |
        (offset & 0xFC);

    asm volatile (
        \\movl %[addr], %%eax
        \\movl %%eax, %[port_addr]
        \\movl %[port_data], %%eax
        : "={eax}" (-> u32),
        : [addr] "r" (address),
          [port_addr] "N" (PCI_CONFIG_ADDR),
          [port_data] "N" (PCI_CONFIG_DATA),
        : "eax"
    );

    return 0; // 实际返回值从 eax 读取
}

pub fn enumerateDevices() void {
    for (0..256) |bus| {
        for (0..32) |slot| {
            const vendor_device = readConfig(@intCast(bus), @intCast(slot), 0, 0);
            const vendor_id = vendor_device & 0xFFFF;

            if (vendor_id != 0xFFFF) {
                const device_id = (vendor_device >> 16) & 0xFFFF;
                // 找到设备
                _ = device_id;
            }
        }
    }
}
