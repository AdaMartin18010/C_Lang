/*
 * Auto-generated from: 06_Thinking_Representation\01_Decision_Trees\08_Type_Conversion_Decision_Tree.md
 * Line: 277
 * Language: c
 * Block ID: aa06e8e7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 违反严格别名
float f = 3.14f;
int i = *(int*)&f;  // 未定义行为

// ✅ 使用union (C11)
union { float f; int i; } u;
u.f = 3.14f;
int i = u.i;  // 合法

// ✅ 使用memcpy
int i;
memcpy(&i, &f, sizeof(i));  // 始终安全
