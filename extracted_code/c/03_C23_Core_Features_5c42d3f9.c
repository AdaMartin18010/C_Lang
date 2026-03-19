/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\03_C23_Core_Features.md
 * Line: 338
 * Language: c
 * Block ID: 5c42d3f9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 二进制字面量
unsigned int flags = 0b1010'1100'0011'0000;

// 位掩码定义更清晰
#define FLAG_READ   0b0001
#define FLAG_WRITE  0b0010
#define FLAG_EXEC   0b0100

// 硬件寄存器操作
uint32_t cr0 = 0b1000'0000'0000'0000'0000'0000'0000'0001;
