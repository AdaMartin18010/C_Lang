/*
 * Auto-generated from: 01_Core_Knowledge_System\04_Standard_Library_Layer\05_C23_Standard_Library_Extensions.md
 * Line: 686
 * Language: c
 * Block ID: aa457e11
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 向上取整到2的幂
unsigned int stdc_bit_ceil_ui (unsigned int value);
unsigned int stdc_bit_ceil_ul (unsigned long value);
unsigned int stdc_bit_ceil_ull(unsigned long long value);

unsigned int stdc_bit_ceil_ui8 (uint8_t  value);
unsigned int stdc_bit_ceil_ui16(uint16_t value);
unsigned int stdc_bit_ceil_ui32(uint32_t value);
unsigned int stdc_bit_ceil_ui64(uint64_t value);

// 向下取整到2的幂
unsigned int stdc_bit_floor_ui (unsigned int value);
unsigned int stdc_bit_floor_ul (unsigned long value);
unsigned int stdc_bit_floor_ull(unsigned long long value);

unsigned int stdc_bit_floor_ui8 (uint8_t  value);
unsigned int stdc_bit_floor_ui16(uint16_t value);
unsigned int stdc_bit_floor_ui32(uint32_t value);
unsigned int stdc_bit_floor_ui64(uint64_t value);

#define stdc_bit_ceil(value)   /* 泛型选择宏 */
#define stdc_bit_floor(value)  /* 泛型选择宏 */
