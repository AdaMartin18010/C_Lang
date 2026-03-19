/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2025\MISRA_C_2025_Rules_Dir11.md
 * Line: 342
 * Language: c
 * Block ID: 7d2deef6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 预期规则方向 [待官方确认] */
/* 限制副作用 */

/* ❌ 违反 [待官方确认] - 多次修改 */
int i = 0;
func(i++, i++);  /* 未定义行为 */

/* ❌ 违反 [待官方确认] */
int x = 0;
x += x++;  /* 未定义行为 */

/* ❌ 违反 [待官方确认] - volatile多次访问 */
volatile uint32_t *reg = (volatile uint32_t *)0x40001000;
uint32_t val = *reg + *reg;  /* 两次读取volatile，可能不同 */

/* ✅ 合规 [待官方确认] */
int i = 0;
int arg1 = i++;
int arg2 = i++;
func(arg1, arg2);

/* ✅ 合规 [待官方确认] */
int x = 0;
x = x + 1;  /* 明确 */

/* ✅ 合规 [待官方确认] */
volatile uint32_t *reg = (volatile uint32_t *)0x40001000;
uint32_t snapshot = *reg;  /* 读取一次 */
uint32_t val = snapshot + snapshot;  /* 使用快照 */
