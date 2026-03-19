/*
 * Auto-generated from: 01_Core_Knowledge_System\02_Core_Layer\03_String_Processing.md
 * Line: 318
 * Language: c
 * Block ID: a61fd37e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 假设1字符=1字节
char *p = "世界";
printf("Length: %zu\n", strlen(p));  // 不是2！是6(UTF-8)
p[1] = 'a';  // 破坏多字节序列！

// ✅ 使用宽字符或UTF-8库
