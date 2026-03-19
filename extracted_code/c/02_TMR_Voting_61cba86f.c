/*
 * Auto-generated from: 04_Industrial_Scenarios\11_Space_Computing\02_TMR_Voting.md
 * Line: 460
 * Language: c
 * Block ID: 61cba86f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 表决器本身无保护
if (a == b || a == c) output = a;
else if (b == c) output = b;
// 表决器故障会导致系统失效

// ✅ 表决器也需要保护
// - 使用硬件表决电路
// - 表决器三模冗余
// - 自检机制
