/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\19_MISRA_C_2023_Rules_19.md
 * Line: 73
 * Language: c
 * Block ID: 4fae001e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 违反 - 参数无括号 */
#define SQUARE(x) x * x
SQUARE(a + b);  /* a + b * a + b，不是(a+b)*(a+b) */

/* ❌ 违反 - 宏体无括号 */
#define SUM(a, b) a + b
SUM(x, y) * 2;  /* x + y * 2，不是(x+y)*2 */

/* ❌ 违反 - 多语句宏无do-while */
#define SWAP(a, b) \
    temp = (a); \
    (a) = (b); \
    (b) = temp;

if (condition)
    SWAP(x, y);  /* 只有第一条在if中！ */

/* ❌ 违反 - 自增/自减在宏参数 */
#define MAX(a, b) ((a) > (b) ? (a) : (b))
int m = MAX(x++, y++);  /* x++或y++可能执行两次！*/
