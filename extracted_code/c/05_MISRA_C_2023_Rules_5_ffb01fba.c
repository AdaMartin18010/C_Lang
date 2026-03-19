/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\05_MISRA_C_2023_Rules_5.md
 * Line: 325
 * Language: c
 * Block ID: ffb01fba
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 违反 - 双精度转单精度丢失精度 */
double precise = 1.0000000001;
float approx = precise;  /* 精度丢失 */

/* ❌ 违反 - 大double转float溢出 */
double big = 1e40;
float small = big;  /* 变成Inf */

/* ❌ 违反 - 浮点转整数溢出 */
double large = 1e20;
int i = large;  /* 未定义行为！ */
