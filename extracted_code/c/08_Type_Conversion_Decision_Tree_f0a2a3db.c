/*
 * Auto-generated from: 06_Thinking_Representation\01_Decision_Trees\08_Type_Conversion_Decision_Tree.md
 * Line: 294
 * Language: c
 * Block ID: f0a2a3db
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 不可移植
int *p = malloc(100);
int addr = (int)p;  // 64位系统截断！

// ✅ 使用intptr_t
#include <stdint.h>
intptr_t addr = (intptr_t)p;  // 可移植
