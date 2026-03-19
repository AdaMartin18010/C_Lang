/*
 * Auto-generated from: 01_Core_Knowledge_System\01_Basic_Layer\02_Data_Type_System.md
 * Line: 263
 * Language: c
 * Block ID: 13faf57b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// UNSAFE: 假设 int 是32位
int buffer[1000000];  // 可能在16位系统上溢出

// UNSAFE: 指针转int（在64位系统截断）
int addr = (int)&variable;  // 64位指针截断为32位

// UNSAFE: 使用sizeof判断位宽（返回字节数）
_Static_assert(sizeof(int) * 8 == 32, "假设int为32位失败");  // 可能失败
