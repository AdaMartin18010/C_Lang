/*
 * Auto-generated from: 01_Core_Knowledge_System\04_Standard_Library_Layer\01_C89_Library.md
 * Line: 293
 * Language: c
 * Block ID: ed5c22a1
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 无边界限制
char buf[10];
scanf("%s", buf);  // 可溢出

// ✅ 限制宽度
scanf("%9s", buf);  // 最多读取9字符+\0

// ✅ 使用动态分配（POSIX扩展）
char *buf;
scanf("%ms", &buf);  // m修饰符：自动malloc
free(buf);
