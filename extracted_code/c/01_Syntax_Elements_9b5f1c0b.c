/*
 * Auto-generated from: 01_Core_Knowledge_System\01_Basic_Layer\01_Syntax_Elements.md
 * Line: 566
 * Language: c
 * Block ID: 9b5f1c0b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 错误
char c = "A";  // char* 转 char，警告或错误

// ✅ 正确
char c = 'A';  // 字符常量
const char *s = "A";  // 字符串（含'\0'）
