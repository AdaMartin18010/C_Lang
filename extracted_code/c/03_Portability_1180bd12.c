/*
 * Auto-generated from: 01_Core_Knowledge_System\06_Advanced_Layer\03_Portability.md
 * Line: 91
 * Language: c
 * Block ID: 1180bd12
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 数据模型：LP64 (Linux/macOS 64-bit)
// char: 8, short: 16, int: 32, long: 64, pointer: 64

// 数据模型：LLP64 (Windows 64-bit)
// char: 8, short: 16, int: 32, long: 32, long long: 64, pointer: 64

// ✅ 可移植：使用定宽类型
#include <stdint.h>
uint32_t reliable_32bit;
uint64_t reliable_64bit;
uintptr_t pointer_sized;

// ✅ 可移植：使用sizeof，不假设
size_t int_size = sizeof(int) * CHAR_BIT;  // 位宽
