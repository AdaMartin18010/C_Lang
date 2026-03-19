/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\15_MISRA_C_2023_Rules_15.md
 * Line: 263
 * Language: c
 * Block ID: ef1f0840
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 违反 - 函数指针转数据指针 */
int func(int x) { return x; }
void *vp = (void *)func;  /* 未定义行为！ */

/* ❌ 违反 - 函数指针转整数 */
int (*fp)(int) = func;
uintptr_t addr = (uintptr_t)fp;  /* 有问题 */

/* ❌ 违反 - 不同类型函数指针 */
int (*fp_int)(int) = func;
void (*fp_void)(void) = (void (*)(void))fp_int;  /* 危险 */
