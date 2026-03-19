/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\21_MISRA_C_2023_Rules_21.md
 * Line: 86
 * Language: c
 * Block ID: 9550cb13
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 加法溢出检查 */
bool safe_add(int a, int b, int *result)
{
    if (b > 0 && a > INT_MAX - b) {
        return false;  /* 正溢出 */
    }
    if (b < 0 && a < INT_MIN - b) {
        return false;  /* 负溢出 */
    }
    *result = a + b;
    return true;
}

/* ✅ 乘法溢出检查 */
bool safe_mul(int a, int b, int *result)
{
    if (a > 0) {
        if (b > 0) {
            if (a > INT_MAX / b) return false;
        } else {
            if (b < INT_MIN / a) return false;
        }
    } else {
        if (b > 0) {
            if (a < INT_MIN / b) return false;
        } else {
            if (a != 0 && b < INT_MAX / a) return false;
        }
    }
    *result = a * b;
    return true;
}

/* ✅ 使用更宽类型 */
int32_t safe_add_32(int32_t a, int32_t b)
{
    int64_t result = (int64_t)a + (int64_t)b;
    if (result > INT32_MAX || result < INT32_MIN) {
        /* 溢出处理 */
    }
    return (int32_t)result;
}
