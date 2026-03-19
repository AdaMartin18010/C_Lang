/*
 * Auto-generated from: 06_Thinking_Representation\01_Decision_Trees\04_Compilation_Error.md
 * Line: 100
 * Language: c
 * Block ID: 5c6892c2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 错误: 类型不匹配
int* p = malloc(100);  // 在C++中错误

// ✅ 修正
int* p = (int*)malloc(100);  // 强制类型转换
