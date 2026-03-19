/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\05_MISRA_C_2023_Rules_5.md
 * Line: 341
 * Language: c
 * Block ID: 4663b9c3
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - 检查范围 */
#include <float.h>

if (precise > FLT_MAX || precise < -FLT_MAX) {
    /* 处理溢出 */
} else if (precise != (double)(float)precise) {
    /* 精度丢失 */
} else {
    float approx = (float)precise;  /* 安全 */
}

/* ✅ 合规 - 检查整数范围 */
if (large > INT_MAX || large < INT_MIN) {
    /* 处理溢出 */
} else {
    int i = (int)large;
}

/* ✅ 合规 - 使用舍入函数 */
#include <math.h>
int rounded = (int)round(f);  /* 正确舍入 */
int truncated = (int)trunc(f);  /* 向零截断 */
