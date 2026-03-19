/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\RPi_Pico\01_Pico_Complete_Guide.md
 * Line: 3518
 * Language: c
 * Block ID: 039d154f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file usb_msc_example.c
 * @brief USB MSC (大容量存储) 示例 - 虚拟 U 盘
 */

#include "pico/stdlib.h"
#include "tusb.h"
#include "class/msc/msc_device.h"

// 虚拟磁盘大小 (64KB)
#define DISK_BLOCK_NUM  128
#define DISK_BLOCK_SIZE 512

static uint8_t msc_disk[DISK_BLOCK_NUM][DISK_BLOCK_SIZE];

// SCSI 命令响应
static bool ejected = false;

// 初始化虚拟磁盘
void msc_disk_init(void) {
    // 清零磁盘
    memset(msc_disk, 0, sizeof(msc_disk));

    // 创建简单的 MBR 和 FAT12 文件系统
    // 这里简化为空磁盘，实际应用需要格式化
}

// MSC 回调函数
int32_t tud_msc_read10_cb(uint8_t lun, uint32_t lba,
                          uint32_t offset, void* buffer,
                          uint32_t bufsize) {
    (void)lun;

    if (lba >= DISK_BLOCK_NUM) return -1;

    uint8_t const* addr = msc_disk[lba] + offset;
    memcpy(buffer, addr, bufsize);

    return (int32_t)bufsize;
}

int32_t tud_msc_write10_cb(uint8_t lun, uint32_t lba,
                           uint32_t offset, uint8_t const* buffer,
                           uint32_t bufsize) {
    (void)lun;

    if (lba >= DISK_BLOCK_NUM) return -1;

    uint8_t* addr = msc_disk[lba] + offset;
    memcpy(addr, buffer, bufsize);

    return (int32_t)bufsize;
}

void tud_msc_inquiry_cb(uint8_t lun, uint8_t vendor_id[8],
                        uint8_t product_id[16],
                        uint8_t product_rev[4]) {
    (void)lun;

    const char vid[] = "Pico";
    const char pid[] = "Mass Storage";
    const char rev[] = "1.0";

    memcpy(vendor_id, vid, strlen(vid));
    memcpy(product_id, pid, strlen(pid));
    memcpy(product_rev, rev, strlen(rev));
}

bool tud_msc_test_unit_ready_cb(uint8_t lun) {
    (void)lun;

    if (ejected) {
        tud_msc_set_sense(lun, SCSI_SENSE_NOT_READY,
                          0x3a, 0x00);
        return false;
    }

    return true;
}

void tud_msc_capacity_cb(uint8_t lun, uint32_t* block_count,
                         uint16_t* block_size) {
    (void)lun;

    *block_count = DISK_BLOCK_NUM;
    *block_size = DISK_BLOCK_SIZE;
}

bool tud_msc_start_stop_cb(uint8_t lun, uint8_t power_condition,
                           bool start, bool load_eject) {
    (void)lun;
    (void)power_condition;

    if (load_eject) {
        if (start) {
            // 加载磁盘
            msc_disk_init();
            ejected = false;
        } else {
            // 弹出磁盘
            ejected = true;
        }
    }

    return true;
}

int32_t tud_msc_scsi_cb(uint8_t lun, uint8_t const scsi_cmd[16],
                        void* buffer, uint16_t bufsize) {
    (void)lun;
    (void)buffer;
    (void)bufsize;

    // 处理 SCSI 命令
    switch (scsi_cmd[0]) {
        case SCSI_CMD_PREVENT_ALLOW_MEDIUM_REMOVAL:
            return 0;
        default:
            tud_msc_set_sense(lun, SCSI_SENSE_ILLEGAL_REQUEST,
                              0x20, 0x00);
            return -1;
    }
}

int main(void) {
    gpio_init(25);
    gpio_set_dir(25, GPIO_OUT);

    msc_disk_init();
    tusb_init();

    while (1) {
        tud_task();

        // 连接状态指示
        static bool last_mounted = false;
        bool mounted = tud_mounted();

        if (mounted != last_mounted) {
            last_mounted = mounted;
            gpio_put(25, mounted);
        }
    }
}
