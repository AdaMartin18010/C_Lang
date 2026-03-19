/*
 * Auto-generated from: 01_Core_Knowledge_System\01_Basic_Layer\IEEE_754_Floating_Point\README.md
 * Line: 31
 * Language: c
 * Block ID: da0e6ff8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 浮点数表示 */
float f = 3.14159f;    /* binary32: 1位符号 + 8位指数 + 23位尾数 */
double d = 3.14159;    /* binary64: 1位符号 + 11位指数 + 52位尾数 */

/* 特殊值 */
float pos_inf = INFINITY;
float neg_inf = -INFINITY;
float nan_val = NAN;
float pos_zero = +0.0f;
float neg_zero = -0.0f;

/* 机器精度 */
#include <float.h>
float eps_f = FLT_EPSILON;    /* 1.192093e-07 */
double eps_d = DBL_EPSILON;   /* 2.220446e-16 */

/* 异常检查 */
#include <fenv.h>
feclearexcept(FE_ALL_EXCEPT);
/* 执行运算... */
if (fetestexcept(FE_OVERFLOW)) {
    /* 处理溢出 */
}
