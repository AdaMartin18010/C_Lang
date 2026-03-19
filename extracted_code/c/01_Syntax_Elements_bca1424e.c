/*
 * Auto-generated from: 01_Core_Knowledge_System\01_Basic_Layer\01_Syntax_Elements.md
 * Line: 379
 * Language: c
 * Block ID: bca1424e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 普通字符串
const char *s1 = "Hello, World!";

// 多行字符串（行拼接）
const char *s2 = "Line 1 "
                 "Line 2 "
                 "Line 3";

// 转义序列
const char *s3 = "Tab:\there\nNew line";

// 原始字符串（C23）
#if __STDC_VERSION__ >= 202311L
const char *raw = R"(Raw "string" without escapes)";
#endif
