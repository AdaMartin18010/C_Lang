/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2025\MISRA_C_2025_Rules_Dir11.md
 * Line: 203
 * Language: c
 * Block ID: f75b5593
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 预期规则方向 [待官方确认] */
/* 函数指针使用限制 */

/* ❌ 违反 [待官方确认] - 函数指针转数据指针 */
int func(int x) { return x; }
void *vp = (void *)func;  /* 未定义行为 */

/* ❌ 违反 [待官方确认] - 不同函数指针类型 */
int (*fp_int)(int) = func;
void (*fp_void)(void) = (void (*)(void))fp_int;  /* 危险 */

/* ✅ 合规 [待官方确认] */
int (*fp)(int) = func;  /* 相同类型 */
int result = fp(42);

/* ✅ 合规 [待官方确认] - 使用typedef */
typedef int (*Operation)(int, int);
int add(int a, int b) { return a + b; }
Operation op = add;
