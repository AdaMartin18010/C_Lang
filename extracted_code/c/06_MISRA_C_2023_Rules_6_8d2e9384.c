/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\06_MISRA_C_2023_Rules_6.md
 * Line: 165
 * Language: c
 * Block ID: 8d2e9384
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 违反 - 多次修改 */
volatile uint32_t *reg = (volatile uint32_t *)0x40001000;
*reg = *reg + 1;  /* 两次访问volatile：读取+写入 */

/* ❌ 违反 - 参数副作用 */
int i = 0;
func(i++, i++);  /* 未定义行为 */

/* ❌ 违反 - 赋值链 */
int a, b;
a = b = 0;  /* 虽然允许，但在MISRA中需谨慎 */

/* ❌ 违反 - 复杂增量 */
int x = 0;
x += x++;  /* 未定义行为 */
