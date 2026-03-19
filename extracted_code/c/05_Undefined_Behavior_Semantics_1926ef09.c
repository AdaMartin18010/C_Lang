/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\00_Core_Semantics_Foundations\05_Undefined_Behavior_Semantics.md
 * Line: 404
 * Language: c
 * Block ID: 1926ef09
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

errno_t strcpy_s(char* dest, rsize_t destsz, const char* src);
// 运行时检查 destsz，溢出时调用约束处理程序
