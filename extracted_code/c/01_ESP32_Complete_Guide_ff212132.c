/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\ESP32\01_ESP32_Complete_Guide.md
 * Line: 135
 * Language: c
 * Block ID: ff212132
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ESP32 内存映射 (以 ESP32-S3 为例)
 *
 * ┌────────────────────────┐ 0x3FF0_0000 (保留)
 * │      外设区域           │ 0x3FF0_0000 - 0x3FF8_0000
 * ├────────────────────────┤
 * │      DRAM (数据RAM)     │ 0x3FF8_0000 - 0x4000_0000
 * │      约 512KB           │
 * ├────────────────────────┤
 * │      IRAM (指令RAM)     │ 0x4000_0000 - 0x4040_0000
 * │      约 256KB           │
 * ├────────────────────────┤
 * │      ROM 代码           │ 0x4000_0000 - 0x4005_0000
 * │      (384KB)            │
 * └────────────────────────┘
 */

/* 内存区域宏定义 */
#define DRAM_START      0x3FF80000
#define DRAM_END        0x40000000
#define IRAM_START      0x40000000
#define IRAM_END        0x40400000
#define ROM_START       0x40000000
#define ROM_END         0x40060000
