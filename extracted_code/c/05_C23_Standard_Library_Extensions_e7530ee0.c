/*
 * Auto-generated from: 01_Core_Knowledge_System\04_Standard_Library_Layer\05_C23_Standard_Library_Extensions.md
 * Line: 377
 * Language: c
 * Block ID: e7530ee0
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

unsigned int stdc_trailing_zeros_ui (unsigned int value);
unsigned int stdc_trailing_zeros_ul (unsigned long value);
unsigned int stdc_trailing_zeros_ull(unsigned long long value);

unsigned int stdc_trailing_zeros_ui8 (uint8_t  value);
unsigned int stdc_trailing_zeros_ui16(uint16_t value);
unsigned int stdc_trailing_zeros_ui32(uint32_t value);
unsigned int stdc_trailing_zeros_ui64(uint64_t value);

unsigned int stdc_trailing_zeros_u8 (uint_least8_t  value);
unsigned int stdc_trailing_zeros_u16(uint_least16_t value);
unsigned int stdc_trailing_zeros_u32(uint_least32_t value);
unsigned int stdc_trailing_zeros_u64(uint_least64_t value);

#define stdc_trailing_zeros(value)  /* 泛型选择宏 */
