/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\15_MISRA_C_2023_Rules_15.md
 * Line: 31
 * Language: c
 * Block ID: 249d1a4b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 违反 - 宽转窄 */
int large = 100000;
short small = large;  /* 可能溢出 */

/* ❌ 违反 - 有符号转无符号 */
int signed_val = -1;
unsigned int unsigned_val = signed_val;  /* 变成大正数 */

/* ❌ 违反 - 浮点转整数 */
float f = 3.7f;
int i = f;  /* 小数丢失 */

/* ❌ 违反 - 双精度转单精度 */
double d = 1.797693e+308;
float f = d;  /* 变成Inf */

/* ❌ 违反 - 函数参数 */
void func(short s);
func(100000);  /* 隐式窄化 */

/* ❌ 违反 - 返回值 */
int64_t calc(void) { return 1LL << 40; }
int result = calc();  /* 截断 */
