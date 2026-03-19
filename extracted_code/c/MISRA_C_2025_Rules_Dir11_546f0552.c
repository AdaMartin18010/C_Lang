/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2025\MISRA_C_2025_Rules_Dir11.md
 * Line: 381
 * Language: c
 * Block ID: 546f0552
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 预期规则方向 [待官方确认] */
/* 使用括号明确优先级 */

/* ❌ 违反 [待官方确认] - 不清楚的优先级 */
int y = a << b + c;  /* 是 (a<<b)+c 还是 a<<(b+c)？ */
bool r = a & b == c;  /* 是 (a&b)==c 还是 a&(b==c)？ */

/* ✅ 合规 [待官方确认] */
int y = a << (b + c);  /* 明确意图 */
bool r = (a & b) == c;  /* 位运算先进行 */

/* ✅ 合规 [待官方确认] - 复杂条件分组 */
if ((a && b) || (c && d))  /* 清晰的逻辑分组 */
if ((a & b) == 0)  /* 位运算结果比较 */
