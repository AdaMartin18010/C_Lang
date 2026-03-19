/*
 * Auto-generated from: 01_Core_Knowledge_System\04_Standard_Library_Layer\05_C23_Standard_Library_Extensions.md
 * Line: 630
 * Language: c
 * Block ID: c0e7b90f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

unsigned int stdc_bit_width_ui (unsigned int value);
unsigned int stdc_bit_width_ul (unsigned long value);
unsigned int stdc_bit_width_ull(unsigned long long value);

unsigned int stdc_bit_width_ui8 (uint8_t  value);
unsigned int stdc_bit_width_ui16(uint16_t value);
unsigned int stdc_bit_width_ui32(uint32_t value);
unsigned int stdc_bit_width_ui64(uint64_t value);

#define stdc_bit_width(value)  /* 泛型选择宏 */
