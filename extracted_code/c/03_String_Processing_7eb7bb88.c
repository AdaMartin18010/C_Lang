/*
 * Auto-generated from: 01_Core_Knowledge_System\02_Core_Layer\03_String_Processing.md
 * Line: 57
 * Language: c
 * Block ID: 7eb7bb88
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C字符串是以空字符'\0'结尾的字符数组

// 字符串字面量（存储在只读数据段）
const char *str1 = "Hello";  // str1指向常量字符串

// 字符数组（可修改）
char str2[] = "Hello";  // 数组大小为6（包含'\0'）
char str3[10] = "Hello";  // 数组大小为10，剩余空间为0

// 手动初始化
char str4[6] = {'H', 'e', 'l', 'l', 'o', '\0'};

// ❌ 常见错误：忘记null终止
char str5[5] = {'H', 'e', 'l', 'l', 'o'};  // 不是字符串！
printf("%s\n", str5);  // 越界读取直到遇到\0
