/*
 * Auto-generated from: 01_Core_Knowledge_System\01_Basic_Layer\03_Operators_Expressions.md
 * Line: 525
 * Language: c
 * Block ID: 4fed9d11
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 移位数量过大（UB）
int x = 1 << 33;  // UB! 超出int宽度

// ❌ 右移负数（实现定义行为）
int y = -1 >> 1;  // 实现定义行为

// ❌ 左移正数变负（UB）
int z = 0x40000000 << 1;  // UB! 符号位改变

// ✅ 安全移位
uint32_t safe = 1U << 31;  // OK

// ✅ 确保移位量在有效范围
#define SHIFT_SAFE(val, shift) \
    ((shift) >= 0 && (shift) < sizeof(val) * CHAR_BIT ? (val) << (shift) : 0)
