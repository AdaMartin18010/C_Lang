/*
 * Auto-generated from: 04_Industrial_Scenarios\01_Automotive_ABS\README.md
 * Line: 1142
 * Language: c
 * Block ID: 962db62d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* safety/defensive_programming.h */
#ifndef DEFENSIVE_PROGRAMMING_H
#define DEFENSIVE_PROGRAMMING_H

/* 编译时断言 */
#define STATIC_ASSERT(condition, msg) \
    typedef char static_assert_##msg[(condition) ? 1 : -1]

/* 运行时断言 - 在发布版本中不禁用 */
#ifdef NDEBUG
    #define RUNTIME_ASSERT(condition) \
        do { \
            if (!(condition)) { \
                enter_safe_state(); \
            } \
        } while (0)
#else
    #define RUNTIME_ASSERT(condition) \
        do { \
            if (!(condition)) { \
                debug_break(); \
                enter_safe_state(); \
            } \
        } while (0)
#endif

/* 范围检查宏 */
#define CHECK_RANGE(value, min, max) \
    (((value) >= (min)) && ((value) <= (max)))

#define CHECK_POINTER(ptr) ((ptr) != NULL)

/* 安全检查包装 */
#define SAFE_DEREF(ptr) \
    (CHECK_POINTER(ptr) ? (ptr) : (enter_safe_state(), (ptr)))

/* 饱和运算 */
static inline uint16_t sat_add_u16(uint16_t a, uint16_t b) {
    uint32_t result = (uint32_t)a + (uint32_t)b;
    return (result > 0xFFFFU) ? 0xFFFFU : (uint16_t)result;
}

static inline int16_t sat_add_s16(int16_t a, int16_t b) {
    int32_t result = (int32_t)a + (int32_t)b;
    if (result > INT16_MAX) return INT16_MAX;
    if (result < INT16_MIN) return INT16_MIN;
    return (int16_t)result;
}

#endif
