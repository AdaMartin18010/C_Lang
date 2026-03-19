/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\Memory_Constrained\01_Memory_Constrained_Design.md
 * Line: 2299
 * Language: c
 * Block ID: a7e0b684
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ============================================================================
 * 位操作工具库
 * ============================================================================ */

#ifndef BITOPS_H
#define BITOPS_H

#include <stdint.h>
#include <stdbool.h>

/* 基本位操作宏 */
#define BIT_SET(reg, bit)      ((reg) |= (1U << (bit)))
#define BIT_CLEAR(reg, bit)    ((reg) &= ~(1U << (bit)))
#define BIT_TOGGLE(reg, bit)   ((reg) ^= (1U << (bit)))
#define BIT_READ(reg, bit)     (((reg) >> (bit)) & 1U)
#define BIT_WRITE(reg, bit, val) \
    ((val) ? BIT_SET(reg, bit) : BIT_CLEAR(reg, bit))

/* 多位操作 */
#define BITS_SET(reg, mask)    ((reg) |= (mask))
#define BITS_CLEAR(reg, mask)  ((reg) &= ~(mask))
#define BITS_TOGGLE(reg, mask) ((reg) ^= (mask))
#define BITS_READ(reg, mask)   ((reg) & (mask))
#define BITS_WRITE(reg, mask, val) \
    ((reg) = ((reg) & ~(mask)) | ((val) & (mask)))

/* 位域操作宏 */
#define BF_PREP(x, name)       (((x) & name##_MASK) << name##_SHIFT)
#define BF_MASK(name)          (((1U << name##_WIDTH) - 1) << name##_SHIFT)
#define BF_GET(y, name)        (((y) & BF_MASK(name)) >> name##_SHIFT)
#define BF_SET(y, x, name)     ((y) = ((y) & ~BF_MASK(name)) | BF_PREP(x, name))

/* 位图操作 */
typedef uint8_t bitmap_t;

static inline void bitmap_set_bit(bitmap_t *map, uint8_t bit)
{
    map[bit / 8] |= (1 << (bit % 8));
}

static inline void bitmap_clear_bit(bitmap_t *map, uint8_t bit)
{
    map[bit / 8] &= ~(1 << (bit % 8));
}

static inline bool bitmap_test_bit(const bitmap_t *map, uint8_t bit)
{
    return (map[bit / 8] & (1 << (bit % 8))) != 0;
}

static inline void bitmap_toggle_bit(bitmap_t *map, uint8_t bit)
{
    map[bit / 8] ^= (1 << (bit % 8));
}

/* 高效位图 - 针对小位图 (<= 32位) */
typedef uint32_t small_bitmap_t;

static inline void small_bitmap_set(small_bitmap_t *map, uint8_t bit)
{
    *map |= (1U << bit);
}

static inline void small_bitmap_clear(small_bitmap_t *map, uint8_t bit)
{
    *map &= ~(1U << bit);
}

static inline bool small_bitmap_test(small_bitmap_t map, uint8_t bit)
{
    return (map & (1U << bit)) != 0;
}

/* 使用GCC内建函数查找第一个置位 */
static inline uint8_t small_bitmap_ffs(small_bitmap_t map)
{
    if (map == 0) return 0;
    return __builtin_ctz(map) + 1;  /* 返回1-based索引 */
}

/* 位反转 */
uint8_t bit_reverse8(uint8_t x)
{
    x = ((x & 0xAA) >> 1) | ((x & 0x55) << 1);
    x = ((x & 0xCC) >> 2) | ((x & 0x33) << 2);
    x = ((x & 0xF0) >> 4) | ((x & 0x0F) << 4);
    return x;
}

/* 计算置位位数 (population count) */
static inline uint8_t popcount8(uint8_t x)
{
    x = (x & 0x55) + ((x >> 1) & 0x55);
    x = (x & 0x33) + ((x >> 2) & 0x33);
    x = (x & 0x0F) + ((x >> 4) & 0x0F);
    return x;
}

static inline uint8_t popcount32(uint32_t x)
{
    /* 使用GCC内建 */
    return __builtin_popcount(x);
}

#endif /* BITOPS_H */
