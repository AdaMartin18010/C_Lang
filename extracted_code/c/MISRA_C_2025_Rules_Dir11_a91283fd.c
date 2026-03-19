/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2025\MISRA_C_2025_Rules_Dir11.md
 * Line: 910
 * Language: c
 * Block ID: a91283fd
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 预期规则方向 [待官方确认] */
/* 内联函数使用 */

/* ✅ 合规 [待官方确认] */
static inline int max(int a, int b) {
    return (a > b) ? a : b;
}

/* ✅ 合规 [待官方确认] - 头文件中 */
/* math_utils.h */
#ifndef MATH_UTILS_H
#define MATH_UTILS_H

static inline int clamp(int value, int min, int max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

#endif

/* ⚠️ 注意 [待官方确认] - 内联是建议不是强制 */
/* 编译器可能选择不内联 */
