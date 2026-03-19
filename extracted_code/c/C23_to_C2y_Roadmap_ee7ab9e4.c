/*
 * Auto-generated from: 00_VERSION_TRACKING\C23_to_C2y_Roadmap.md
 * Line: 130
 * Language: c
 * Block ID: ee7ab9e4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// <stdbit.h> - 位操作工具 (C23)
#include <stdbit.h>

// 计数1的位数
int popcount = stdc_count_ones(0b101101);  // 返回4

// 前导零计数
int leading = stdc_count_zeros(0x0F);  // 返回28 (32位系统)

// 位宽度
int width = stdc_bit_width(0b10110);  // 返回5

// 字节序操作
uint32_t be = stdc_byteswap(0x12345678);  // 返回0x78563412

// ------------------------------------
// <stdckdint.h> - 安全检查整数运算
#include <stdckdint.h>

bool overflow;
int result = ckd_add(&overflow, a, b);
if (overflow) {
    // 处理溢出
}

// ------------------------------------
// <stdfloat.h> - 标准浮点类型 (C23)
#include <stdfloat.h>

float16_t  f16;   // IEEE 754 binary16
float32_t  f32;   // IEEE 754 binary32
float64_t  f64;   // IEEE 754 binary64
float128_t f128;  // IEEE 754 binary128 (如果支持)
