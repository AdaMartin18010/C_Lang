/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2025\MISRA_C_2025_Rules_Dir6.md
 * Line: 492
 * Language: c
 * Block ID: 7243e106
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 预期规则方向 [待官方确认] */
/* 基本类型使用规范 */

/* ❌ 违反 [待官方确认] - 依赖平台类型大小 */
int counter = 0;  /* int大小不确定 */

/* ✅ 合规 [待官方确认] - 使用固定宽度类型 */
#include <stdint.h>

uint32_t counter = 0;  /* 明确32位 */
int32_t signed_counter = 0;  /* 明确32位有符号 */

/* ✅ 合规 [待官方确认] - 字符类型 */
char ch = 'a';  /* 字符值 */
uint8_t byte = 0xFF;  /* 数值 */
