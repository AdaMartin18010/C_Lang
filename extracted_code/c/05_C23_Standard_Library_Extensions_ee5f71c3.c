/*
 * Auto-generated from: 01_Core_Knowledge_System\04_Standard_Library_Layer\05_C23_Standard_Library_Extensions.md
 * Line: 571
 * Language: c
 * Block ID: ee5f71c3
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

bool stdc_has_single_bit_ui (unsigned int value);
bool stdc_has_single_bit_ul (unsigned long value);
bool stdc_has_single_bit_ull(unsigned long long value);

bool stdc_has_single_bit_ui8 (uint8_t  value);
bool stdc_has_single_bit_ui16(uint16_t value);
bool stdc_has_single_bit_ui32(uint32_t value);
bool stdc_has_single_bit_ui64(uint64_t value);

#define stdc_has_single_bit(value)  /* 泛型选择宏 */
