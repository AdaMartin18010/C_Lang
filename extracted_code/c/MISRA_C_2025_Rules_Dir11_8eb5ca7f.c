/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2025\MISRA_C_2025_Rules_Dir11.md
 * Line: 745
 * Language: c
 * Block ID: 8eb5ca7f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 预期规则方向 [待官方确认] */
/* 指针转换限制 */

/* ❌ 违反 [待官方确认] - 对象指针混用 */
int arr[10];
float *fp = (float *)arr;  /* 危险！ */

/* ❌ 违反 [待官方确认] - 整数转指针 */
int *p = (int *)0x40001000;  /* 危险！ */

/* ✅ 合规 [待官方确认] - 使用uintptr_t */
#include <stdint.h>
uintptr_t addr = 0x40001000;
volatile uint32_t *reg = (volatile uint32_t *)addr;

/* ✅ 合规 [待官方确认] - void*转换 */
void *vp = malloc(sizeof(int));
int *ip = (int *)vp;  /* void*允许转换 */
