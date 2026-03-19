/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\20_Industrial_Case_Studies\03_Baremetal_Embedded.md
 * Line: 884
 * Language: c
 * Block ID: cc1349ce
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// pmp.c - 物理内存保护单元配置

#include "hal.h"

#define PMP_NCFG 16  // PMP配置寄存器数量

// PMP地址模式
typedef enum {
    PMP_OFF = 0,
    PMP_TOR = 1,  // Top of Range
    PMP_NA4 = 2,  // Naturally aligned 4-byte
    PMP_NAPOT = 3 // Naturally aligned power-of-2
} pmp_a_field_t;

// PMP权限
#define PMP_R (1 << 0)  // 读
#define PMP_W (1 << 1)  // 写
#define PMP_X (1 << 2)  // 执行
#define PMP_A (3 << 3)  // 地址模式
#define PMP_L (1 << 7)  // 锁定

// 设置PMP区域 (TOR模式)
void pmp_set_region_tor(int index, uintptr_t start, uintptr_t end, uint8_t perm) {
    if (index >= PMP_NCFG) return;

    // 设置地址寄存器
    uintptr_t addr = end >> 2;

    switch (index) {
        case 0: write_csr(pmpaddr0, addr); break;
        case 1: write_csr(pmpaddr1, addr); break;
        case 2: write_csr(pmpaddr2, addr); break;
        case 3: write_csr(pmpaddr3, addr); break;
        // ... 更多寄存器
    }

    // 设置配置
    uint64_t cfg = (perm & 0xF) | ((index == 0) ? 0 : PMP_TOR << 3);
    uint64_t cfg_mask = 0xFFULL << (index * 8);
    uint64_t cfg_val = ((uint64_t)cfg) << (index * 8);

    uint64_t pmpcfg0 = read_csr(pmpcfg0);
    pmpcfg0 = (pmpcfg0 & ~cfg_mask) | cfg_val;
    write_csr(pmpcfg0, pmpcfg0);
}

// 设置PMP区域 (NAPOT模式)
void pmp_set_region_napot(int index, uintptr_t base, size_t size, uint8_t perm) {
    if (index >= PMP_NCFG || (size & (size - 1)) != 0) return;

    // NAPOT编码: base | ((size/4) - 1)
    uintptr_t napot_addr = (base >> 2) | ((size >> 3) - 1);

    switch (index) {
        case 0: write_csr(pmpaddr0, napot_addr); break;
        case 1: write_csr(pmpaddr1, napot_addr); break;
        case 2: write_csr(pmpaddr2, napot_addr); break;
        case 3: write_csr(pmpaddr3, napot_addr); break;
        // ...
    }

    uint64_t cfg = (perm & 0xF) | (PMP_NAPOT << 3);
    uint64_t cfg_mask = 0xFFULL << (index * 8);
    uint64_t cfg_val = ((uint64_t)cfg) << (index * 8);

    uint64_t pmpcfg0 = read_csr(pmpcfg0);
    pmpcfg0 = (pmpcfg0 & ~cfg_mask) | cfg_val;
    write_csr(pmpcfg0, pmpcfg0);
}

// 典型PMP配置
void pmp_init(void) {
    // 区域0: 整个地址空间 - 无权限 (默认拒绝)
    pmp_set_region_tor(0, 0, -1UL, 0);

    // 区域1: RAM区域 - RW
    pmp_set_region_napot(1, 0x80000000, 0x20000000, PMP_R | PMP_W);

    // 区域2: Flash区域 - RX
    pmp_set_region_napot(2, 0x20000000, 0x00800000, PMP_R | PMP_X);

    // 区域3: 外设区域 - RW
    pmp_set_region_napot(3, 0x10000000, 0x10000000, PMP_R | PMP_W);
}
