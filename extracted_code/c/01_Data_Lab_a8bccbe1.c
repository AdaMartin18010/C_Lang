/*
 * Auto-generated from: 04_Industrial_Scenarios\17_CMU_15_213_Labs\01_Data_Lab.md
 * Line: 38
 * Language: c
 * Block ID: a8bccbe1
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 基本位操作符
&   // 按位与
|   // 按位或
^   // 按位异或
~   // 按位取反
<<  // 左移
>>  // 右移（算术右移对有符号数）

// 常用位操作技巧
// 1. 设置第n位：x | (1 << n)
// 2. 清除第n位：x & ~(1 << n)
// 3. 翻转第n位：x ^ (1 << n)
// 4. 检查第n位：(x >> n) & 1
