/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\06_MISRA_C_2023_Rules_6.md
 * Line: 185
 * Language: c
 * Block ID: 7d735fba
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - 分离volatile访问 */
volatile uint32_t *reg = (volatile uint32_t *)0x40001000;
uint32_t value = *reg;  /* 读取 */
*reg = value + 1;       /* 写入 */

/* ✅ 合规 - 副作用分离 */
int i = 0;
int arg1 = i++;
int arg2 = i++;
func(arg1, arg2);

/* ✅ 合规 - 明确赋值 */
int a = 0;
int b = 0;

/* ✅ 合规 - 明确增量 */
int x = 0;
x = x + 1;  /* 明确 */
