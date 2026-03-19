/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\05_MISRA_C_2023_Rules_5.md
 * Line: 268
 * Language: c
 * Block ID: ee6d55d9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 违反 - 大转小 */
int large = 100000;
short small = large;  /* 可能溢出 */

/* ❌ 违反 - 有符号转无符号 */
int signed_val = -1;
unsigned int unsigned_val = signed_val;  /* 变成大正数！ */

/* ❌ 违反 - 浮点转整数 */
float f = 3.7f;
int i = f;  /* 丢失小数部分，i=3 */

/* ❌ 违反 - 函数参数隐式转换 */
void func(uint16_t x);
func(100000);  /* 隐式转换，值被截断 */

/* ❌ 违反 - 返回类型不匹配 */
int64_t calc(void) {
    return 1LL << 40;
}
int result = calc();  /* 截断！ */
