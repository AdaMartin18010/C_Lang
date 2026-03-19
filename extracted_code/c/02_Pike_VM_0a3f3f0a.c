/*
 * Auto-generated from: 03_System_Technology_Domains\02_Regex_Engine\02_Pike_VM.md
 * Line: 47
 * Language: c
 * Block ID: 0a3f3f0a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 传统回溯引擎（如PCRE）的问题
// 模式: (a+)+b
// 输入: aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaac
// 时间复杂度: 指数级 O(2^n)

// 原因: 每次遇到 + 都要做选择，失败时回溯
// a+ 可以匹配 1个a, 2个a, ... n个a
// 外层 (a+)+ 也有多种选择
// 组合爆炸！
