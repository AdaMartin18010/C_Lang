/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\21_MISRA_C_2023_Rules_21.md
 * Line: 143
 * Language: c
 * Block ID: dc6aeea9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 无符号下溢检查 */
bool safe_sub_unsigned(unsigned int a, unsigned int b, unsigned int *result)
{
    if (b > a) {
        return false;  /* 下溢 */
    }
    *result = a - b;
    return true;
}

/* ✅ 有符号下溢检查 */
bool safe_sub_signed(int a, int b, int *result)
{
    if (b > 0 && a < INT_MIN + b) {
        return false;  /* 负溢出 */
    }
    *result = a - b;
    return true;
}

/* ✅ 浮点下溢检查 */
#include <float.h>

bool safe_float_mul(float a, float b, float *result)
{
    *result = a * b;
    if (fabsf(*result) < FLT_MIN && *result != 0.0f) {
        /* 下溢到次正规数 */
        return false;
    }
    return true;
}
