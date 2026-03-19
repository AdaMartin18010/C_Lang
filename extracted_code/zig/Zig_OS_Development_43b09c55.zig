//
// Auto-generated from: Zig\Zig_OS_Development.md
// Line: 185
// Language: zig
// Block ID: 43b09c55
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// uefi_boot.zig - UEFI 启动入口
const std = @import("std");
const uefi = std.os.uefi;

pub fn main() noreturn {
    // 初始化 UEFI 服务
    const con_out = uefi.system_table.con_out.?;
    _ = con_out.outputString("Hello from Zig UEFI!\r\n");

    // 获取内存映射
    var memory_map_size: usize = 0;
    var memory_map: ?[*]uefi.tables.MemoryDescriptor = null;
    var map_key: usize = 0;
    var descriptor_size: usize = 0;
    var descriptor_version: u32 = 0;

    _ = uefi.system_table.boot_services.?.getMemoryMap(
        &memory_map_size,
        memory_map,
        &map_key,
        &descriptor_size,
        &descriptor_version,
    );

    // 退出 UEFI 启动服务，进入内核
    _ = uefi.system_table.boot_services.?.exitBootServices(
        uefi.handle,
        map_key,
    );

    // 跳转到内核入口
    kernel_main();

    while (true) {
        asm volatile ("hlt");
    }
}

fn kernel_main() void {
    // 内核初始化
}
