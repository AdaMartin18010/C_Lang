/*
 * Auto-generated from: 01_Core_Knowledge_System\04_Standard_Library_Layer\05_C23_Standard_Library_Extensions.md
 * Line: 237
 * Language: c
 * Block ID: c0b81e3c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 标准整数类型版本
unsigned int stdc_leading_zeros_ui (unsigned int value);
unsigned int stdc_leading_zeros_ul (unsigned long value);
unsigned int stdc_leading_zeros_ull(unsigned long long value);

// 精确宽度整数类型版本
unsigned int stdc_leading_zeros_ui8 (uint8_t  value);
unsigned int stdc_leading_zeros_ui16(uint16_t value);
unsigned int stdc_leading_zeros_ui32(uint32_t value);
unsigned int stdc_leading_zeros_ui64(uint64_t value);

// 最小宽度整数类型版本（条件支持）
unsigned int stdc_leading_zeros_u8 (uint_least8_t  value);
unsigned int stdc_leading_zeros_u16(uint_least16_t value);
unsigned int stdc_leading_zeros_u32(uint_least32_t value);
unsigned int stdc_leading_zeros_u64(uint_least64_t value);

// 通用宏（泛型选择，C23 _Generic）
#define stdc_leading_zeros(value)  /* 根据value类型选择合适函数 */
