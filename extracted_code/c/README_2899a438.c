/*
 * Auto-generated from: 04_Industrial_Scenarios\02_Avionics_Systems\README.md
 * Line: 1494
 * Language: c
 * Block ID: 2899a438
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file example_compliant_code.c
 * @brief 符合 DO-178C / MISRA C 的示例代码
 */

/* ============================================================================
 * 1. 头文件保护
 * ============================================================================
 * Req: MISRA C:2012 Rule 20.5
 */
#ifndef EXAMPLE_COMPLIANT_CODE_H
#define EXAMPLE_COMPLIANT_CODE_H

/* ============================================================================
 * 2. 使用固定宽度类型
 * ============================================================================
 * Req: MISRA C:2012 Dir 4.6
 */
#include <stdint.h>
#include <stdbool.h>

/* ============================================================================
 * 3. 避免使用宏定义常量，使用枚举或const
 * ============================================================================
 * Req: MISRA C:2012 Dir 4.9
 */
enum {
    MAX_SENSOR_COUNT = 8,
    MAX_ACTUATOR_COUNT = 16,
    SAMPLING_PERIOD_MS = 10
};

static const uint32_t WATCHDOG_TIMEOUT_MS = 100U;

/* ============================================================================
 * 4. 函数声明明确可访问性
 * ============================================================================
 * Req: DO-178C 结构化设计
 */

/**
 * @brief 初始化传感器模块
 * @return true 如果初始化成功
 */
extern bool Sensor_Init(void);

/**
 * @brief 读取传感器值
 * @param sensor_id 传感器ID (0..MAX_SENSOR_COUNT-1)
 * @param value 输出值指针
 * @return true 如果读取成功
 */
extern bool Sensor_Read(uint8_t sensor_id, int32_t *value);

/* ============================================================================
 * 5. 避免使用隐式类型转换
 * ============================================================================
 * Req: MISRA C:2012 Rule 10.1, 10.3, 10.4
 */
static inline uint32_t safe_add_u32(uint32_t a, uint32_t b, bool *overflow) {
    uint32_t result = a + b;
    *overflow = (result < a); /* 检查溢出 */
    return result;
}

/* ============================================================================
 * 6. 所有路径必须有明确的返回值
 * ============================================================================
 * Req: MISRA C:2012 Rule 15.5
 */
static inline int32_t clamp_int32(int32_t value, int32_t min, int32_t max) {
    int32_t result;

    if (value < min) {
        result = min;
    } else if (value > max) {
        result = max;
    } else {
        result = value;
    }

    return result;
}

/* ============================================================================
 * 7. 避免使用危险函数
 * ============================================================================
 * Req: MISRA C:2012 Rule 21.6, 21.8
 * 禁止使用: malloc/free, sprintf, strcpy, strcat 等
 */

/* 使用安全的字符串复制 */
static inline void safe_strcpy(char *dest, size_t dest_size, const char *src) {
    size_t i;
    for (i = 0U; (i < (dest_size - 1U)) && (src[i] != '\0'); i++) {
        dest[i] = src[i];
    }
    dest[i] = '\0';
}

/* ============================================================================
 * 8. 防御性编程 - 参数检查
 * ============================================================================
 * Req: DO-178C 鲁棒性要求
 */
#define CHECK_PARAM(cond, error_code) \
    do { \
        if (!(cond)) { \
            Error_Report(error_code, __FILE__, __LINE__); \
            return false; \
        } \
    } while(0)

/* ============================================================================
 * 9. 无死代码，无不可达代码
 * ============================================================================
 * Req: MISRA C:2012 Rule 2.1, 2.2
 */

/* ============================================================================
 * 10. 限制函数复杂度（圈复杂度）
 * ============================================================================
 * Req: DO-178C DAL A: 圈复杂度 <= 10
 */

#endif /* EXAMPLE_COMPLIANT_CODE_H */
