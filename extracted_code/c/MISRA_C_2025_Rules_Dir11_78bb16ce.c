/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2025\MISRA_C_2025_Rules_Dir11.md
 * Line: 850
 * Language: c
 * Block ID: 78bb16ce
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 预期规则方向 [待官方确认] */
/* 函数调用安全 */

/* ❌ 违反 [待官方确认] - 可变参数类型错误 */
printf("%d", 3.14);  /* 类型不匹配，UB */

/* ❌ 违反 [待官方确认] - 参数数量错误 */
void func(int a, int b);
func(1);  /* 参数不足 */
func(1, 2, 3);  /* 参数过多 */

/* ✅ 合规 [待官方确认] */
printf("%f", 3.14);  /* 正确格式 */
printf("%d", (int)3.14);  /* 明确转换 */

/* ✅ 合规 [待官方确认] */
void func(int a, int b);
func(1, 2);  /* 参数匹配 */
