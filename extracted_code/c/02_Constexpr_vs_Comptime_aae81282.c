/*
 * Auto-generated from: 08_Zig_C_Connection\06_C23_Zig_Parallel\02_Constexpr_vs_Comptime.md
 * Line: 2401
 * Language: c
 * Block ID: aae81282
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C23: 查找表生成
#include <math.h>
#include <stdint.h>

#define TABLE_SIZE 256

// 方法 1：手动计算（繁琐且易错）
constexpr uint8_t CRC8_TABLE_MANUAL[TABLE_SIZE] = {
    0x00, 0x07, 0x0E, 0x09, 0x1C, 0x1B, 0x12, 0x15,
    // ... 需要计算 256 个值
};

// 方法 2：使用 constexpr 函数计算单个值
constexpr uint8_t crc8_byte(uint8_t byte) {
    uint8_t crc = byte;
    for (int i = 0; i < 8; i++) {
        crc = (crc << 1) ^ ((crc & 0x80) ? 0x07 : 0);
    }
    return crc;
}

// ❌ C23 无法从 constexpr 函数生成完整数组
// 只能在运行时填充

void init_crc_table(uint8_t* table) {
    for (int i = 0; i < TABLE_SIZE; i++) {
        table[i] = crc8_byte(i);
    }
}

// 方法 3：外部代码生成（最实用）
// 使用 Python/脚本预先生成表
// #include "generated_crc_table.h"
