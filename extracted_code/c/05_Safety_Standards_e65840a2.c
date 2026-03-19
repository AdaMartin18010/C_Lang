/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\05_Safety_Standards.md
 * Line: 155
 * Language: c
 * Block ID: e65840a2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 违反规则：隐式转换 (Rule 10.1, 10.3)
uint8_t u8 = 255;
uint16_t u16 = u8 << 1;  // 先提升为int，可能溢出

// ✅ 合规代码 (Rule 10.5)
uint16_t u16 = (uint16_t)((uint16_t)u8 << 1);

// ❌ 违反规则：不兼容指针转换 (Rule 11.3)
int *p = malloc(100);  // malloc返回void*

// ✅ 合规代码 (Rule 11.5)
int *p = (int *)malloc(100 * sizeof(int));

// ❌ 违反规则：指针和整数转换 (Rule 11.4)
int addr = (int)&x;  // 禁止！

// ✅ 合规代码：如果需要地址，保持为指针类型
