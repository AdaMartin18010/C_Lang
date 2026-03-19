/*
 * Auto-generated from: 01_Core_Knowledge_System\01_Basic_Layer\IEEE_754_Floating_Point\01_IEEE_754_Basics.md
 * Line: 210
 * Language: c
 * Block ID: 8c47a4e7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 特殊值的位模式 */

/* +0.0 */
float positive_zero = 0.0f;  /* 0x00000000 */

/* -0.0 */
float negative_zero = -0.0f; /* 0x80000000 */

/* +Inf */
float positive_infinity = 1.0f / 0.0f;  /* 0x7F800000 */

/* -Inf */
float negative_infinity = -1.0f / 0.0f; /* 0xFF800000 */

/* NaN (Not a Number) */
float nan_value = 0.0f / 0.0f;  /* 0x7FC00000 (quiet NaN) */

/* 检查特殊值的代码 */
#include <math.h>
#include <stdbool.h>

bool is_positive_zero(float f)
{
    return f == 0.0f && !signbit(f);
}

bool is_negative_zero(float f)
{
    return f == 0.0f && signbit(f);
}

bool is_infinity(float f)
{
    return isinf(f);
}

bool is_nan(float f)
{
    return isnan(f);
}

bool is_subnormal(float f)
{
    return fpclassify(f) == FP_SUBNORMAL;
}
