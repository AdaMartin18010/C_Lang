/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\19_Hardware_Debugging\01_JTAG_Debug.md
 * Line: 2230
 * Language: c
 * Block ID: 8d5f7cf3
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// mpu_config.c
// 使用 MPU 检测内存越界

#include <stdint.h>

// MPU 区域配置
void MPU_Config(void) {
    // 禁用 MPU
    __asm volatile ("dsb");
    MPU->CTRL = 0;

    // 区域0: Flash (只读)
    MPU->RNR = 0;
    MPU->RBAR = 0x08000000 | 0x10;  // 地址 + 有效位
    MPU->RASR = (0x11 << 1) |       // 大小: 512KB (2^(17+1))
                (0x06 << 24) |      // 访问权限: 只读
                (1 << 0);           // 使能

    // 区域1: RAM (读写)
    MPU->RNR = 1;
    MPU->RBAR = 0x20000000 | 0x10;
    MPU->RASR = (0x0E << 1) |       // 大小: 128KB
                (0x03 << 24) |      // 访问权限: 读写
                (1 << 0);

    // 区域2: 栈保护区（红区）
    MPU->RNR = 2;
    MPU->RBAR = 0x2001F000 | 0x10;  // 栈底部
    MPU->RASR = (0x04 << 1) |       // 大小: 4KB
                (0x00 << 24) |      // 访问权限: 无
                (1 << 0);

    // 使能 MPU
    MPU->CTRL = 1 |                 // 使能 MPU
                (1 << 2);           // 使能 HardFault 对内存故障
    __asm volatile ("dsb");
    __asm volatile ("isb");
}
