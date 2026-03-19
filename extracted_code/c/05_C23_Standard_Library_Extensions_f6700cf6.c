/*
 * Auto-generated from: 01_Core_Knowledge_System\04_Standard_Library_Layer\05_C23_Standard_Library_Extensions.md
 * Line: 1892
 * Language: c
 * Block ID: f6700cf6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C11风格（仍然有效）
#include <stdnoreturn.h>

_Noreturn void fatal_error(const char *msg);

// C23风格：使用标准属性
[[noreturn]] void fatal_error_c23(const char *msg);

// 或者（如果支持简化属性语法）
[noreturn] void fatal_error_alt(const char *msg);
