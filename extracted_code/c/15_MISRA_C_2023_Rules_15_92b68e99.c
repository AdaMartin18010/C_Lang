/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\15_MISRA_C_2023_Rules_15.md
 * Line: 59
 * Language: c
 * Block ID: 92b68e99
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - 显式转换并检查 */
if (large > SHRT_MAX || large < SHRT_MIN) {
    /* 错误处理 */
} else {
    short small = (short)large;  /* 安全 */
}

/* ✅ 合规 - 显式转换 */
unsigned int unsigned_val = (unsigned int)signed_val;  /* 明确意图 */

/* ✅ 合规 - 显式截断 */
int i = (int)f;  /* 明确截断小数 */
int rounded = (int)round(f);  /* 正确舍入 */

/* ✅ 合规 - 检查浮点范围 */
#include <float.h>
if (d > FLT_MAX || d < -FLT_MAX) {
    /* 溢出处理 */
} else {
    float f = (float)d;
}

/* ✅ 合规 - 使用匹配类型 */
void func(short s);
short value = (short)100000;  /* 先转换 */
func(value);

/* ✅ 合规 - 返回类型匹配 */
int64_t result64 = calc();
int32_t result32;
if (result64 > INT32_MAX || result64 < INT32_MIN) {
    /* 溢出处理 */
} else {
    result32 = (int32_t)result64;
}
