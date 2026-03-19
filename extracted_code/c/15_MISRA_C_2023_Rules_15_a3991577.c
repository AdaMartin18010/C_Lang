/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\15_MISRA_C_2023_Rules_15.md
 * Line: 349
 * Language: c
 * Block ID: a3991577
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 违反 - 通过指针类型双关 */
float f = 3.14f;
int32_t bits = *(int32_t *)&f;  /* 严格别名违规 */

/* ❌ 违反 - 复杂类型双关 */
struct Data d;
uint32_t *words = (uint32_t *)&d;  /* 不安全 */
