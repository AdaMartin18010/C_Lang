//
// Auto-generated from: Zig\Zig_OS_Development.md
// Line: 851
// Language: zig
// Block ID: 7fda69fc
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// fat32.zig - FAT32 文件系统
const std = @import("std");

pub const BootSector = packed struct {
    jmp_boot: [3]u8,
    oem_name: [8]u8,
    bytes_per_sector: u16,
    sectors_per_cluster: u8,
    reserved_sector_count: u16,
    num_fats: u8,
    root_entry_count: u16,
    total_sectors_16: u16,
    media: u8,
    fat_size_16: u16,
    sectors_per_track: u16,
    num_heads: u16,
    hidden_sectors: u32,
    total_sectors_32: u32,
    fat_size_32: u32,
    ext_flags: u16,
    fs_version: u16,
    root_cluster: u32,
    fs_info: u16,
    backup_boot_sec: u16,
    reserved: [12]u8,
    drv_num: u8,
    reserved1: u8,
    boot_sig: u8,
    volume_id: u32,
    volume_label: [11]u8,
    file_system_type: [8]u8,
};

pub const FAT32 = struct {
    boot: BootSector,
    fat: []u32,
    data_start: usize,

    pub fn clusterToSector(self: *const FAT32, cluster: u32) usize {
        return self.data_start + (cluster - 2) * self.boot.sectors_per_cluster;
    }

    pub fn readCluster(self: *FAT32, cluster: u32, buf: []u8) void {
        const sector = self.clusterToSector(cluster);
        _ = sector;
        _ = buf;
        // 从磁盘读取扇区
    }
};
