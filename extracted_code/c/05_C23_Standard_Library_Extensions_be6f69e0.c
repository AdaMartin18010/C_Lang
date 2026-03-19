/*
 * Auto-generated from: 01_Core_Knowledge_System\04_Standard_Library_Layer\05_C23_Standard_Library_Extensions.md
 * Line: 146
 * Language: c
 * Block ID: be6f69e0
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 8位 - 实际为恒等函数，用于完整性
uint8_t  stdc_byteswap_ui8 (uint8_t  value);
uint8_t  stdc_byteswap_u8  (uint8_t  value);   // 类型支持宏启用时

// 16位
uint16_t stdc_byteswap_ui16(uint16_t value);
uint16_t stdc_byteswap_u16 (uint16_t value);   // 类型支持宏启用时

// 32位
uint32_t stdc_byteswap_ui32(uint32_t value);
uint32_t stdc_byteswap_u32 (uint32_t value);   // 类型支持宏启用时

// 64位
uint64_t stdc_byteswap_ui64(uint64_t value);
uint64_t stdc_byteswap_u64 (uint64_t value);   // 类型支持宏启用时

// 最大宽度无符号整数
uint_leastN_t stdc_byteswap_uint_leastN(uint_leastN_t value);  // N = 8, 16, 32, 64
