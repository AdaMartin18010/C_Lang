/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2025\MISRA_C_2025_Rules_Dir11.md
 * Line: 405
 * Language: c
 * Block ID: d91398cc
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 预期规则方向 [待官方确认] */
/* 限制表达式复杂度 */

/* ❌ 违反 [待官方确认] - 过度复杂 */
result = (a + b) * (c - d) / ((e + f) * (g - h));

/* ❌ 违反 [待官方确认] - 嵌套太深 */
x = a ? (b ? (c ? d : e) : f) : g;

/* ✅ 合规 [待官方确认] - 分步计算 */
int sum1 = a + b;
int diff1 = c - d;
int product = sum1 * diff1;
int sum2 = e + f;
int diff2 = g - h;
result = product / (sum2 * diff2);

/* ✅ 合规 [待官方确认] - 使用临时变量 */
float temp1 = calculate_temp1(a, b);
float temp2 = calculate_temp2(c, d);
result = combine(temp1, temp2);
