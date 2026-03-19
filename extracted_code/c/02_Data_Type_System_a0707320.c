/*
 * Auto-generated from: 01_Core_Knowledge_System\01_Basic_Layer\02_Data_Type_System.md
 * Line: 231
 * Language: c
 * Block ID: a0707320
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C标准只保证相对大小：
// sizeof(char) <= sizeof(short) <= sizeof(int) <= sizeof(long) <= sizeof(long long)
// 且 char 至少 8位，short/int 至少 16位，long 至少 32位，long long 至少 64位

// 实际典型实现（LP64数据模型）：
// char        : 8 位, 范围 [-128, 127] 或 [0, 255]
// short       : 16位, 范围 [-32768, 32767]
// int         : 32位, 范围 [-2147483648, 2147483647]
// long        : 64位, 范围 [-9223372036854775808, 9223372036854775807]
// long long   : 64位, 同上
// size_t      : 与指针同宽，LP64下为64位无符号
