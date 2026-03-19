/*
 * Auto-generated from: 01_Core_Knowledge_System\04_Standard_Library_Layer\C23_Standard_Library\01_stdbit_h_Complete_Reference.md
 * Line: 320
 * Language: c
 * Block ID: 6d4557b8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// <stdbit.h> 完整 API

// 人口计数
unsigned int       stdc_count_ones_ui(unsigned int x);
unsigned long      stdc_count_ones_ul(unsigned long x);
unsigned long long stdc_count_ones_ull(unsigned long long x);

// 前导零
unsigned int       stdc_leading_zeros_ui(unsigned int x);
unsigned long      stdc_leading_zeros_ul(unsigned long x);
unsigned long long stdc_leading_zeros_ull(unsigned long long x);
unsigned int       stdc_leading_ones_ui(unsigned int x);
unsigned long      stdc_leading_ones_ul(unsigned long x);
unsigned long long stdc_leading_ones_ull(unsigned long long x);

// 尾随零
unsigned int       stdc_trailing_zeros_ui(unsigned int x);
unsigned long      stdc_trailing_zeros_ul(unsigned long x);
unsigned long long stdc_trailing_zeros_ull(unsigned long long x);
unsigned int       stdc_trailing_ones_ui(unsigned int x);
unsigned long      stdc_trailing_ones_ul(unsigned long x);
unsigned long long stdc_trailing_ones_ull(unsigned long long x);

// 位宽
unsigned int       stdc_bit_width_ui(unsigned int x);
unsigned long      stdc_bit_width_ul(unsigned long x);
unsigned long long stdc_bit_width_ull(unsigned long long x);

// 位反转
unsigned char      stdc_bit_reverse_uint8(unsigned char x);
unsigned short     stdc_bit_reverse_uint16(unsigned short x);
unsigned int       stdc_bit_reverse_uint32(unsigned int x);
unsigned long long stdc_bit_reverse_uint64(unsigned long long x);

// 字节序
unsigned short     stdc_byteorder_bswap16(unsigned short x);
unsigned int       stdc_byteorder_bswap32(unsigned int x);
unsigned long long stdc_byteorder_bswap64(unsigned long long x);
