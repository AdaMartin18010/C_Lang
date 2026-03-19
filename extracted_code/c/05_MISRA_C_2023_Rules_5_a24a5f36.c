/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\05_MISRA_C_2023_Rules_5.md
 * Line: 166
 * Language: c
 * Block ID: a24a5f36
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 违反 - 移除const */
const int value = 10;
int *p = (int *)&value;  /* 移除const！ */
*p = 20;  /* 未定义行为！ */

/* ❌ 违反 - 移除volatile */
volatile uint32_t *status_reg = (volatile uint32_t *)0x40001000;
uint32_t *p = (uint32_t *)status_reg;  /* 移除volatile */
/* 编译器可能优化掉对此地址的访问 */

/* ❌ 违反 - 移除restrict */
void func(int * restrict p, int * restrict q);
int *p1, *p2;
func((int *)p1, (int *)p2);  /* 如果p1/p2有restrict，被移除 */
