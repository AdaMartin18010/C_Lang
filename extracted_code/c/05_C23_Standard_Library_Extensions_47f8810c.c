/*
 * Auto-generated from: 01_Core_Knowledge_System\04_Standard_Library_Layer\05_C23_Standard_Library_Extensions.md
 * Line: 317
 * Language: c
 * Block ID: 47f8810c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

unsigned int stdc_leading_ones_ui (unsigned int value);
unsigned int stdc_leading_ones_ul (unsigned long value);
unsigned int stdc_leading_ones_ull(unsigned long long value);

unsigned int stdc_leading_ones_ui8 (uint8_t  value);
unsigned int stdc_leading_ones_ui16(uint16_t value);
unsigned int stdc_leading_ones_ui32(uint32_t value);
unsigned int stdc_leading_ones_ui64(uint64_t value);

// 最小宽度类型版本
unsigned int stdc_leading_ones_u8 (uint_least8_t  value);
unsigned int stdc_leading_ones_u16(uint_least16_t value);
unsigned int stdc_leading_ones_u32(uint_least32_t value);
unsigned int stdc_leading_ones_u64(uint_least64_t value);

#define stdc_leading_ones(value)  /* 泛型选择宏 */
