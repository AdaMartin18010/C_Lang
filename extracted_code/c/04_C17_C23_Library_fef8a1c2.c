/*
 * Auto-generated from: 01_Core_Knowledge_System\04_Standard_Library_Layer\04_C17_C23_Library.md
 * Line: 92
 * Language: c
 * Block ID: fef8a1c2
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
static_assert(sizeof(void*) == 8, "Require 64-bit pointers");
