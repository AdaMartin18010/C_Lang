/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\13_Hardware_Description\04_HLS_with_C\01_Vivado_HLS_Introduction.md
 * Line: 381
 * Language: c
 * Block ID: 006231fc
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

//===========================================
// config.h - 设计配置头文件
//===========================================
#ifndef __CONFIG_H__
#define __CONFIG_H__

// 设计参数（可在综合前修改）
#define DATA_WIDTH      16
#define COEFF_WIDTH     16
#define TAP_SIZE        64
#define INPUT_SIZE      1024

// 数据类型定义
#include "ap_fixed.h"
typedef ap_fixed<DATA_WIDTH, DATA_WIDTH-4> data_t;
typedef ap_fixed<COEFF_WIDTH, 2> coeff_t;
typedef ap_fixed<DATA_WIDTH+COEFF_WIDTH+6, 10> accum_t;

// 优化开关
#define USE_PIPELINE    1
#define USE_UNROLL      1
#define UNROLL_FACTOR   8

#endif
