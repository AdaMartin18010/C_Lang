/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\02_C17_C23_Features.md
 * Line: 54
 * Language: c
 * Block ID: 901a7e3c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C11: _Static_assert 需要消息参数
_Static_assert(sizeof(int) == 4, "int must be 32-bit");

// C17: 消息参数可选
_Static_assert(sizeof(int) == 4);  // 合法

// C23: 简写为 static_assert
static_assert(sizeof(int) == 4);
