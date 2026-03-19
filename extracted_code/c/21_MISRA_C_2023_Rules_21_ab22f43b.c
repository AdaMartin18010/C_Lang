/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\21_MISRA_C_2023_Rules_21.md
 * Line: 44
 * Language: c
 * Block ID: ab22f43b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - 检查除数 */
int safe_divide(int a, int b, int *result)
{
    if (b == 0) {
        return ERROR_DIV_BY_ZERO;
    }
    *result = a / b;
    return SUCCESS;
}

/* ✅ 合规 - 浮点除法 */
float safe_float_divide(float a, float b)
{
    if (fabsf(b) < FLT_EPSILON) {
        return INFINITY;  /* 或错误处理 */
    }
    return a / b;
}

/* ✅ 合规 - 模运算检查 */
int safe_modulo(int a, int b)
{
    if (b == 0) {
        return ERROR;
    }
    return a % b;
}
