/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\00_Quick_Reference_Guide.md
 * Line: 260
 * Language: c
 * Block ID: 7dc2254f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 错误 */
int x = i++ + i++;  /* 未定义行为 */

/* ✅ 修复 */
int temp1 = i++;
int temp2 = i++;
int x = temp1 + temp2;
