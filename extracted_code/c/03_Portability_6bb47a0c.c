/*
 * Auto-generated from: 01_Core_Knowledge_System\06_Advanced_Layer\03_Portability.md
 * Line: 197
 * Language: c
 * Block ID: 6bb47a0c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 依赖char为有符号
char c = 200;  // ARM上可能是56（回绕），x86上可能是-56

// ✅ 明确指定
unsigned char c = 200;  // 总是200

// ❌ 依赖sizeof(int) = 4
int arr[1000000];  // 可能在16位系统溢出

// ✅ 使用size_t
size_t max_elements = SIZE_MAX / sizeof(int);
