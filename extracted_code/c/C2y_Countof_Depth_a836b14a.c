/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Countof_Depth.md
 * Line: 178
 * Language: c
 * Block ID: a836b14a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

char str1[] = "Hello";           // 数组大小为6（包含'\0'）
_Countof(str1);                  // 返回 6

char str2[100] = "Hi";
_Countof(str2);                  // 返回 100（不是3！）

const char *str3 = "Hello";
// _Countof(str3);               // 编译错误：str3是指针，不是数组
