/*
 * Auto-generated from: 00_VERSION_TRACKING\C2y_New_Proposals_2026.md
 * Line: 193
 * Language: c
 * Block ID: 6af829cd
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C2y 提案 (N3657)

// 基本闭包
int multiplier = 10;
auto fn = [](int x) { return x * multiplier; };  // 捕获multiplier
int result = fn(5);  // 50

// 显式捕获
int a = 1, b = 2;
auto sum = `[a, b](void) { return a + b; };`
