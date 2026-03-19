/*
 * Auto-generated from: 01_Core_Knowledge_System\08_Application_Domains\02_Embedded_Physical_Control.md
 * Line: 415
 * Language: c
 * Block ID: 32cb7fcf
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================
// 位操作 = 对硬件寄存器的精确控制
// ============================================================

// 基本操作语义
#define BIT(n)          (1U << (n))                 // 第n位为1
#define BITS(m, n)      (((1U << ((m)-(n)+1)) - 1) << (n))  // m到n位为1
#define SET_BIT(x, b)   ((x) |= BIT(b))              // 置位
#define CLEAR_BIT(x, b) ((x) &= ~BIT(b))             // 清零
#define TOGGLE_BIT(x,b) ((x) ^= BIT(b))              // 翻转
#define READ_BIT(x, b)  (((x) >> (b)) & 1U)          // 读取

// 字段操作 (多bit配置)
#define FIELD_MASK(pos, width)  (((1U << (width)) - 1) << (pos))
#define SET_FIELD(reg, pos, width, val) do { \
    (reg) &= ~FIELD_MASK(pos, width); \
    (reg) |= ((val) << (pos)) & FIELD_MASK(pos, width); \
} while(0)
#define GET_FIELD(reg, pos, width) (((reg) >> (pos)) & ((1U << (width)) - 1))

// 使用示例: 配置ADC分辨率 (bits 24-25, 2位宽)
// 00=12bit, 01=10bit, 10=8bit, 11=6bit
SET_FIELD(ADC1->CR1, 24, 2, 2);  // 设置为8bit分辨率
uint32_t res = GET_FIELD(ADC1->CR1, 24, 2);  // 读取当前分辨率
