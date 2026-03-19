/*
 * Auto-generated from: 01_Core_Knowledge_System\01_Basic_Layer\01_Syntax_Elements.md
 * Line: 298
 * Language: c
 * Block ID: 9c962ae2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 十进制
int dec = 42;
int dec_negative = -42;

// 八进制（0开头）
int oct = 052;      // 42 in decimal

// 十六进制（0x/0X开头）
int hex = 0x2A;     // 42 in decimal
int hex_upper = 0X2a;

// 二进制（C23，0b/0B开头）
#if __STDC_VERSION__ >= 202311L
int bin = 0b101010;  // 42 in decimal
int bin_sep = 0b1010'1010;  // 单引号分隔符(C23)
#endif

// 整数后缀
unsigned int u = 42U;
long l = 42L;
unsigned long ul = 42UL;
long long ll = 42LL;
unsigned long long ull = 42ULL;
