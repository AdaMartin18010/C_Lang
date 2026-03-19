/*
 * Auto-generated from: 06_Thinking_Representation\08_Index\04_Modern_C_Quick_Reference.md
 * Line: 169
 * Language: c
 * Block ID: 15127989
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C11前: 编译期检查技巧
typedef char static_assert_failed[CONDITION ? 1 : -1];

// C11起
static_assert(sizeof(int) >= 4, "int太小");

// C23
static_assert(sizeof(int) >= 4);  // 消息可选
