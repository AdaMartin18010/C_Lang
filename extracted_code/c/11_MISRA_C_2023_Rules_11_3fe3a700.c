/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\11_MISRA_C_2023_Rules_11.md
 * Line: 93
 * Language: c
 * Block ID: 3fe3a700
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 违反 - 不清楚的优先级 */
int x = a + b * c;  /* 知道优先级，但... */
int y = a << b + c;  /* 是 (a<<b)+c 还是 a<<(b+c)？ */
bool r = a & b == c;  /* 是 (a&b)==c 还是 a&(b==c)？ */
int z = a ? b : c + d;  /* 是 (a?b:c)+d 还是 a?b:(c+d)？*/

/* ❌ 违反 - 混合运算符 */
if (a && b || c && d)  /* 虽然正确，但可读性差 */
if (a & b == 0)  /* 错误：先比较b==0 */
if (a + b << c)  /* 移位优先级低于加法 */
