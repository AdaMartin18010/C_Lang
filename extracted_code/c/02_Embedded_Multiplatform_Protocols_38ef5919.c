/*
 * Auto-generated from: 01_Core_Knowledge_System\08_Application_Domains\02_Embedded_Multiplatform_Protocols.md
 * Line: 233
 * Language: c
 * Block ID: 38ef5919
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================
// 内存映射对比
// ============================================================

// ========== STM32F4内存映射 ==========
// 0x0000 0000 - 0x000F FFFF: 代码区 (别名区，可映射到Flash/SRAM)
// 0x0800 0000 - 0x080F FFFF: Flash (1MB)
// 0x1FFF 0000 - 0x1FFF 77FF: 系统存储器 (Bootloader)
// 0x2000 0000 - 0x2001 FFFF: SRAM1 (128KB)
// 0x2002 0000 - 0x2003 FFFF: SRAM2 (128KB)
// 0x4000 0000 - 0x5FFF FFFF: 外设区
// 0xE000 0000 - 0xE00F FFFF: Cortex-M4内部外设

// 链接脚本关键部分
MEMORY {
    RAM (rwx) : ORIGIN = 0x20000000, LENGTH = 128K
    FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 1024K
}

// ========== ESP32内存映射 ==========
// ESP32使用虚拟内存映射，通过MMU访问外部Flash
// 0x3F40 0000 - 0x3F7F FFFF: 外部Flash缓存 (通过MMU映射)
// 0x3FF8 0000 - 0x3FFF FFFF: DRAM (520KB)
// 0x4000 0000 - 0x400C 1FFF: 内部ROM
// 0x400C 2000 - 0x40BF FFFF: 外部Flash指令缓存

// ESP32内存分配特殊性
// 使用heap_caps分配不同区域的内存
void *ptr = heap_caps_malloc(size, MALLOC_CAP_DMA);  // DMA-capable内存
void *iram = heap_caps_malloc(size, MALLOC_CAP_32BIT); // 32位对齐，可执行

// ========== RP2040内存映射 ==========
// 0x0000 0000 - 0x3FFF FFFF: XIP (Flash映射，通过QSPI)
// 0x2000 0000 - 0x2003 FFFF: SRAM (264KB)
// 0x4000 0000 - 0x4001 3FFF: APB外设
// 0x4001 4000 - 0x4001 7FFF: AHB外设
// 0x4002 0000 - 0x4002 0FFF: SIO (单周期I/O)
// 0xD000 0000 - 0xD000 01FF: Cortex-M0+内部外设

// RP2040的SRAM分为6个bank，支持并行访问
// Bank0-3: 通用
// Bank4-5: 可单独断电（低功耗）
