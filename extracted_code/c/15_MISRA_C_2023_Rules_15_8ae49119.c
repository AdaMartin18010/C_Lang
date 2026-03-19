/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\15_MISRA_C_2023_Rules_15.md
 * Line: 171
 * Language: c
 * Block ID: 8ae49119
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - 显式浮点转换 */
double d = 3.14159;
float f = (float)d;  /* 显式 */

/* ✅ 合规 - 显式转整数 */
float f = 3.7f;
int i = (int)f;  /* 截断 */
int rounded = (int)round(f);  /* 舍入 */

/* ✅ 合规 - 检查精度丢失 */
float original = 1.23456789f;
double extended = (double)original;  /* 安全，精度增加 */
float back = (float)extended;  /* 可能丢失精度 */
if (original != back) {
    /* 精度丢失 */
}
