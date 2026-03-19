/*
 * Auto-generated from: 04_Industrial_Scenarios\01_Automotive_ABS\01_ABS_System.md
 * Line: 226
 * Language: c
 * Block ID: b3481baa
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 滑移率计算 - 工程实现 */
#define SLIP_RATIO_MIN        0.0f    /* 纯滚动 */
#define SLIP_RATIO_OPT_MIN    0.10f   /* 最佳区间下限 */
#define SLIP_RATIO_OPT_MAX    0.20f   /* 最佳区间上限 */
#define SLIP_RATIO_CRITICAL   0.30f   /* 临界抱死 */
#define SLIP_RATIO_LOCK       1.0f    /* 完全抱死 */
