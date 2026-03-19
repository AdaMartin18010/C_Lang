/*
 * Auto-generated from: 03_System_Technology_Domains\02_Regex_Engine\01_Thompson_NFA.md
 * Line: 338
 * Language: c
 * Block ID: 8f8412b4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 回溯实现（最坏指数时间）
// (a*)* 匹配 aaaaa... 会爆炸

// ✅ Thompson NFA（线性时间）
// 每次字符处理都是O(状态数)
