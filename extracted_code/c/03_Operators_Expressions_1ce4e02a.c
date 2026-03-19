/*
 * Auto-generated from: 01_Core_Knowledge_System\01_Basic_Layer\03_Operators_Expressions.md
 * Line: 580
 * Language: c
 * Block ID: 1ce4e02a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 错误：位运算优先级低于关系运算
if (flags & MASK == VALUE)  // 实际解析为：flags & (MASK == VALUE)

// ✅ 正确
if ((flags & MASK) == VALUE)

// ❌ 错误：移位优先级低于加减
int x = value << 2 + 1;  // 实际解析为：value << (2 + 1)

// ✅ 正确
int x = (value << 2) + 1;

// ❌ 错误：条件运算符优先级问题
int a = condition ? x : y + 1;  // 解析为：condition ? x : (y + 1)

// ✅ 如果意图是 (condition ? x : y) + 1
int a = (condition ? x : y) + 1;
