/*
 * Auto-generated from: 04_Industrial_Scenarios\17_CMU_15_213_Labs\03_Attack_Lab.md
 * Line: 554
 * Language: c
 * Block ID: 6709bcd4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 危险函数替代
gets()      → fgets(buf, sizeof(buf), stdin)
strcpy()    → strncpy()
strcat()    → strncat()
sprintf()   → snprintf()

// 使用安全编译选项
// gcc -fstack-protector-all -D_FORTIFY_SOURCE=2 -Wformat-security
