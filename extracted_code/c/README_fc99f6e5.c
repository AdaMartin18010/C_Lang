/*
 * Auto-generated from: 07_Modern_Toolchain\12_Modern_C_Standards\README.md
 * Line: 158
 * Language: c
 * Block ID: fc99f6e5
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* stdio.h - 格式化宏 */
#include <inttypes.h>
uint64_t val = 123;
printf("%" PRIu64 "\n", val);  /* 可移植的格式 */

/* 复数数学 */
#include <complex.h>
double complex c = cexp(I * PI);

/* 浮点环境控制 */
#include <fenv.h>
fesetround(FE_UPWARD);

/* 数学函数扩展 */
#include <math.h>
double result = round(2.5);  /* 四舍五入 */
double truncated = trunc(2.9);
