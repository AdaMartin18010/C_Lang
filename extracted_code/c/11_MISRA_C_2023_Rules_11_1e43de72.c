/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\11_MISRA_C_2023_Rules_11.md
 * Line: 108
 * Language: c
 * Block ID: 1e43de72
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - 明确的括号 */
int x = a + (b * c);  /* 即使知道优先级，也显式 */
int y = a << (b + c);  /* 明确意图 */
bool r = (a & b) == c;  /* 位运算先进行 */
int z = a ? b : (c + d);  /* 明确三元运算范围 */

/* ✅ 合规 - 复杂条件分组 */
if ((a && b) || (c && d))  /* 清晰的逻辑分组 */
if ((a & b) == 0)  /* 位运算结果比较 */
if ((a + b) << c)  /* 加法后移位 */

/* ✅ 合规 - 多层嵌套 */
result = ((a * b) + (c * d)) / ((e - f) + 1);

/* ✅ 合规 - 位运算 */
flags = (flags & ~MASK) | (value << SHIFT);
