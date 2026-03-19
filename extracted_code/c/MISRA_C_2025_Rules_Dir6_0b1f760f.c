/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2025\MISRA_C_2025_Rules_Dir6.md
 * Line: 275
 * Language: c
 * Block ID: 0b1f760f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 预期规则方向 [待官方确认] */
/* 函数声明必须原型化 */

/* ❌ 违反 [待官方确认] */
void func();  /* 旧式声明，无参数检查 */
func(1, 2, 3);  /* 编译器不检查 */

/* ✅ 合规 [待官方确认] */
void func(int a, int b);  /* 原型声明 */
void func(int a, int b) { /* ... */ }  /* 定义 */

/* ✅ 合规 [待官方确认] - 无参数函数 */
void func(void);  /* 明确表示无参数 */
