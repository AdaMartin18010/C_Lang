/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\08_MISRA_C_2023_Rules_8.md
 * Line: 95
 * Language: c
 * Block ID: e42b5bd7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 违反 - 参数无括号 */
#define SQUARE(x) x * x
SQUARE(a + b);  /* 展开: a + b * a + b，不是(a+b)*(a+b) */

/* ❌ 违反 - 宏体无括号 */
#define DOUBLE(x) (x) + (x)
DOUBLE(x) * 2;  /* 展开: (x)+(x)*2，不是((x)+(x))*2 */

/* ❌ 违反 - 复杂表达式 */
#define MAX(a, b) a > b ? a : b
MAX(x, y) + 1;  /* 优先级问题 */
