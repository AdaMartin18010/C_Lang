/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\21_MISRA_C_2023_Rules_21.md
 * Line: 209
 * Language: c
 * Block ID: b4f95588
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - 无符号移位 */
uint32_t safe_shift_left(uint32_t value, unsigned int shift)
{
    if (shift >= 32) {
        return 0;  /* 或错误处理 */
    }
    return value << shift;
}

uint32_t safe_shift_right(uint32_t value, unsigned int shift)
{
    if (shift >= 32) {
        return 0;
    }
    return value >> shift;
}

/* ✅ 合规 - 使用宏定义 */
#define SET_BIT(value, bit) ((value) | (1U << (bit)))
#define CLEAR_BIT(value, bit) ((value) & ~(1U << (bit)))
#define TEST_BIT(value, bit) (((value) >> (bit)) & 1U)

/* ✅ 合规 - 循环移位 */
uint32_t rotate_left(uint32_t value, unsigned int shift)
{
    shift &= 31;  /* 限制在0-31 */
    return (value << shift) | (value >> (32 - shift));
}
