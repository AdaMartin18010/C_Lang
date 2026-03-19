/*
 * Auto-generated from: 04_Industrial_Scenarios\01_Automotive_ABS\README.md
 * Line: 1201
 * Language: c
 * Block ID: 232526a0
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* mpu/mpu_config.c */
#include "mpu/mpu_config.h"

/* ARM Cortex-M4 MPU 寄存器 */
#define MPU_BASE            0xE000ED90
#define MPU_TYPER          (*(volatile uint32_t*)(MPU_BASE + 0x00))
#define MPU_CTRL           (*(volatile uint32_t*)(MPU_BASE + 0x04))
#define MPU_RNR            (*(volatile uint32_t*)(MPU_BASE + 0x08))
#define MPU_RBAR           (*(volatile uint32_t*)(MPU_BASE + 0x0C))
#define MPU_RASR           (*(volatile uint32_t*)(MPU_BASE + 0x10))

/* 区域属性 */
#define MPU_ATTR_XN         (1 << 28)   /* 禁止执行 */
#define MPU_ATTR_AP_RW_RW   (3 << 24)   /* 特权读写, 非特权读写 */
#define MPU_ATTR_AP_RW_RO   (2 << 24)   /* 特权读写, 非特权只读 */
#define MPU_ATTR_AP_RW_NONE (1 << 24)   /* 特权读写, 非特权无访问 */
#define MPU_ATTR_AP_RW_RW_NP (0 << 24)  /* 特权读写 */
#define MPU_ATTR_CB_WB_WA   (3 << 16)   /* Write-Back, Write-Allocate */
#define MPU_ATTR_CB_WT      (2 << 16)   /* Write-Through */
#define MPU_ATTR_CB_WB      (1 << 16)   /* Write-Back, no Write-Allocate */
#define MPU_ATTR_CB_NON     (0 << 16)   /* Non-cacheable */
#define MPU_ATTR_S          (1 << 18)   /* 共享 */

/* MPU 区域配置表 */
typedef struct {
    uint32_t base_address;
    uint32_t size;          /* 2^n 字节, n >= 4 */
    uint32_t attributes;
    bool enable;
} mpu_region_config_t;

/* 典型嵌入式系统 MPU 配置 */
static const mpu_region_config_t g_mpu_config[] = {
    /* 区域 0: Flash - 只读执行 */
    {
        .base_address = 0x08000000,
        .size = 512 * 1024,  /* 512KB */
        .attributes = MPU_ATTR_AP_RW_RO | MPU_ATTR_CB_WT | MPU_ATTR_S,
        .enable = true
    },
    /* 区域 1: RAM - 读写, 禁止执行 */
    {
        .base_address = 0x20000000,
        .size = 128 * 1024,  /* 128KB */
        .attributes = MPU_ATTR_XN | MPU_ATTR_AP_RW_RW | MPU_ATTR_CB_WB_WA | MPU_ATTR_S,
        .enable = true
    },
    /* 区域 2: 外设 - 禁止缓存, 禁止执行 */
    {
        .base_address = 0x40000000,
        .size = 512 * 1024,  /* 512KB */
        .attributes = MPU_ATTR_XN | MPU_ATTR_AP_RW_RW_NP | MPU_ATTR_CB_NON | MPU_ATTR_S,
        .enable = true
    },
    /* 区域 3: 栈保护 - 不可访问 (作为哨兵) */
    {
        .base_address = 0x2001F000,  /* 栈底 */
        .size = 4 * 1024,  /* 4KB 保护区域 */
        .attributes = MPU_ATTR_XN | MPU_ATTR_AP_RW_NONE | MPU_ATTR_CB_NON,
        .enable = true
    }
};

void mpu_init(void) {
    /* 禁用 MPU */
    MPU_CTRL = 0;

    uint32_t region_count = sizeof(g_mpu_config) / sizeof(g_mpu_config[0]);

    for (uint32_t i = 0; i < region_count && i < 8; i++) {
        const mpu_region_config_t* cfg = &g_mpu_config[i];

        /* 选择区域 */
        MPU_RNR = i;

        /* 配置基地址 */
        uint32_t rbar = cfg->base_address | (i << 4) | (cfg->enable ? 1 : 0);
        MPU_RBAR = rbar;

        /* 计算大小编码: 2^(SIZE+1) 字节, SIZE >= 4 */
        uint32_t size_encoding = 0;
        uint32_t temp_size = cfg->size;
        while (temp_size > 1) {
            temp_size >>= 1;
            size_encoding++;
        }
        size_encoding -= 1;  /* MPU 编码: 2^(SIZE+1) */

        /* 配置属性和大小 */
        uint32_t rasr = (size_encoding << 1) | cfg->attributes | (cfg->enable ? 1 : 0);
        MPU_RASR = rasr;
    }

    /* 启用 MPU (带默认内存映射背景区域) */
    MPU_CTRL = 0x05;  /* ENABLE | HFNMIENA */

    /* 内存屏障 */
    __DSB();
    __ISB();
}

/* 栈溢出检测 */
bool mpu_check_stack_overflow(void) {
    /* 检查 MPU 故障状态 */
    uint32_t cfsr = *(volatile uint32_t*)0xE000ED28;

    if (cfsr & 0x80) {  /* MMARVALID: 内存管理故障地址有效 */
        uint32_t mmar = *(volatile uint32_t*)0xE000ED34;

        /* 检查是否访问了栈保护区 */
        if (mmar >= 0x2001F000 && mmar < 0x20020000) {
            return true;
        }
    }

    return false;
}
