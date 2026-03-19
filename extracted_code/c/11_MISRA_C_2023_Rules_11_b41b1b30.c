/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\11_MISRA_C_2023_Rules_11.md
 * Line: 333
 * Language: c
 * Block ID: b41b1b30
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - 使用无符号类型 */
uint32_t flags = 0x80;
uint32_t bit = flags >> 31;  /* 逻辑移位，结果确定 */

/* ✅ 合规 - 先扩展为无符号 */
uint8_t byte = 0x80;
uint32_t extended = ((uint32_t)byte) << 8;  /* 0x00008000 */

/* ✅ 合规 - 无符号取反 */
uint32_t mask = ~0U;  /* 0xFFFFFFFF */

/* ✅ 合规 - 掩码操作 */
#define FLAG_A (1U << 0)
#define FLAG_B (1U << 1)
#define FLAG_C (1U << 2)

uint32_t status = 0;
status |= FLAG_A;  /* 设置位 */
status &= ~FLAG_B; /* 清除位 */
bool has_flag = (status & FLAG_C) != 0;  /* 测试位 */

/* ✅ 合规 - 寄存器操作 */
volatile uint32_t *ctrl_reg = (uint32_t *)0x40001000;
*ctrl_reg |= (1U << 3);   /* 设置第3位 */
*ctrl_reg &= ~(1U << 4);  /* 清除第4位 */
