/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\09_Physical_Machine_Layer\05_Memory_Management_Hardware\04_Memory_Protection_Unit.md
 * Line: 842
 * Language: c
 * Block ID: ccbec29c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * ARM Cortex-M MPU 完整编程接口
 */

#ifndef MPU_H
#define MPU_H

#include <stdint.h>

// MPU寄存器结构体
typedef struct {
    volatile uint32_t TYPE;     // 0xE000ED90
    volatile uint32_t CTRL;     // 0xE000ED94
    volatile uint32_t RNR;      // 0xE000ED98
    volatile uint32_t RBAR;     // 0xE000ED9C
    volatile uint32_t RASR;     // 0xE000EDA0
} MPU_Type;

#define MPU_BASE        0xE000ED90
#define MPU             ((MPU_Type *)MPU_BASE)

// RASR寄存器位定义
#define RASR_XN_Pos             28
#define RASR_AP_Pos             24
#define RASR_TEX_Pos            19
#define RASR_S_Pos              18
#define RASR_C_Pos              17
#define RASR_B_Pos              16
#define RASR_SRD_Pos            8
#define RASR_SIZE_Pos           1
#define RASR_ENABLE_Pos         0

// 访问权限定义
#define RASR_AP_NO_ACCESS       (0x0 << RASR_AP_Pos)
#define RASR_AP_PRIV_RW         (0x1 << RASR_AP_Pos)
#define RASR_AP_PRIV_RW_USER_RO (0x2 << RASR_AP_Pos)
#define RASR_AP_PRIV_RW_USER_RW (0x3 << RASR_AP_Pos)
#define RASR_AP_PRIV_RO         (0x5 << RASR_AP_Pos)
#define RASR_AP_PRIV_RO_USER_RO (0x6 << RASR_AP_Pos)
#define RASR_AP_PRIV_RO_USER_RW (0x7 << RASR_AP_Pos)

// 内存类型定义
#define RASR_STRONGLY_ORDERED   ((0x0 << RASR_TEX_Pos) | (0 << RASR_C_Pos) | (0 << RASR_B_Pos))
#define RASR_SHAREABLE_DEVICE   ((0x0 << RASR_TEX_Pos) | (0 << RASR_C_Pos) | (1 << RASR_B_Pos))
#define RASR_WRITE_THROUGH      ((0x0 << RASR_TEX_Pos) | (1 << RASR_C_Pos) | (0 << RASR_B_Pos))
#define RASR_WRITE_BACK         ((0x0 << RASR_TEX_Pos) | (1 << RASR_C_Pos) | (1 << RASR_B_Pos))
#define RASR_NON_CACHEABLE      ((0x1 << RASR_TEX_Pos) | (0 << RASR_C_Pos) | (0 << RASR_B_Pos))
#define RASR_WRITE_BACK_ALLOC   ((0x1 << RASR_TEX_Pos) | (1 << RASR_C_Pos) | (1 << RASR_B_Pos))

/*
 * 初始化MPU
 */
void mpu_init(void) {
    // 禁用MPU
    MPU->CTRL = 0;

    // 检查MPU是否可用
    uint32_t type = MPU->TYPE;
    uint32_t dregion = (type >> 8) & 0xFF;

    if (dregion == 0) {
        // MPU不可用
        return;
    }

    // 禁用所有区域
    for (uint32_t i = 0; i < dregion; i++) {
        MPU->RNR = i;
        MPU->RASR = 0;
    }
}

/*
 * 启用MPU
 */
void mpu_enable(uint32_t privdefena, uint32_t hfnmiena) {
    uint32_t ctrl = MPU_CTRL_ENABLE_Msk;

    if (privdefena) {
        ctrl |= MPU_CTRL_PRIVDEFENA_Msk;
    }

    if (hfnmiena) {
        ctrl |= MPU_CTRL_HFNMIENA_Msk;
    }

    MPU->CTRL = ctrl;

    // 内存屏障
    __DSB();
    __ISB();
}

/*
 * 禁用MPU
 */
void mpu_disable(void) {
    MPU->CTRL = 0;
    __DSB();
    __ISB();
}

/*
 * 配置MPU区域 (完整版)
 */
void mpu_config_region_full(uint32_t region_num,
                            uint32_t base_addr,
                            uint32_t size_log2,  // 实际大小 = 2^(size_log2+1)
                            uint32_t attributes,
                            uint32_t srd_mask,   // 子区域禁用掩码
                            uint32_t enable) {
    // 选择区域
    MPU->RNR = region_num;

    // 设置基址
    // VALID=1, REGION=0 (使用RNR选择的区域)
    MPU->RBAR = (base_addr & 0xFFFFFFE0) | 0x10;

    // 设置属性和大小
    MPU->RASR = (attributes) |
                ((srd_mask & 0xFF) << RASR_SRD_Pos) |
                ((size_log2 & 0x1F) << RASR_SIZE_Pos) |
                (enable ? 1 : 0);
}

/*
 * 便捷的MPU配置宏
 */
#define MPU_CONFIG_CODE(addr, size) \
    mpu_config_region_full(region++, addr, size, \
        RASR_AP_PRIV_RO_USER_RO | RASR_WRITE_THROUGH, 0, 1)

#define MPU_CONFIG_DATA(addr, size) \
    mpu_config_region_full(region++, addr, size, \
        RASR_AP_PRIV_RW_USER_RW | RASR_WRITE_BACK | (1 << RASR_XN_Pos), 0, 1)

#define MPU_CONFIG_DEVICE(addr, size) \
    mpu_config_region_full(region++, addr, size, \
        RASR_AP_PRIV_RW | RASR_SHAREABLE_DEVICE | (1 << RASR_XN_Pos), 0, 1)

#define MPU_CONFIG_STRONGLY_ORDERED(addr, size) \
    mpu_config_region_full(region++, addr, size, \
        RASR_AP_PRIV_RW | RASR_STRONGLY_ORDERED | (1 << RASR_XN_Pos), 0, 1)

#endif // MPU_H
