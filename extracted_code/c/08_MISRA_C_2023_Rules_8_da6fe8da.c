/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\08_MISRA_C_2023_Rules_8.md
 * Line: 111
 * Language: c
 * Block ID: da6fe8da
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - 完全括号化 */
#define SQUARE(x) ((x) * (x))
SQUARE(a + b);  /* 展开: ((a+b)*(a+b)) */

/* ✅ 合规 - 整个表达式括号化 */
#define DOUBLE(x) ((x) + (x))
DOUBLE(x) * 2;  /* 展开: ((x)+(x))*2 */

/* ✅ 合规 - 条件表达式完全括号化 */
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
MAX(x, y) + 1;  /* 展开: (((x)>(y))?(x):(y))+1 */

/* ✅ 合规 - 多语句宏 */
#define SWAP(a, b) do { \
    typeof(a) temp = (a); \
    (a) = (b); \
    (b) = temp; \
} while(0)
