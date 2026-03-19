/*
 * Auto-generated from: 04_Industrial_Scenarios\02_Automotive_ECU\03_Functional_Safety.md
 * Line: 577
 * Language: c
 * Block ID: 049e5be6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file safety_library.c
 * @brief ASIL-D级别安全函数库
 * @note 所有函数包含输入验证和错误处理
 */

#include <stdint.h>
#include <stdbool.h>
#include <limits.h>

/* 多样性比较标记 */
#define DIVERSITY_TAG_A     (0xA5A5A5A5U)
#define DIVERSITY_TAG_B     (0x5A5A5A5AU)

/**
 * @brief 安全整数加法 - 带溢出检测
 * @param a 操作数1
 * @param b 操作数2
 * @param result 结果输出
 * @return true: 成功, false: 溢出
 */
bool Safety_Add_Int32(int32_t a, int32_t b, int32_t *result) {
    /* 空指针检查 */
    if (NULL == result) {
        return false;
    }

    /* 溢出检测 */
    if ((b > 0) && (a > (INT32_MAX - b))) {
        /* 正溢出 */
        return false;
    }

    if ((b < 0) && (a < (INT32_MIN - b))) {
        /* 负溢出 */
        return false;
    }

    *result = a + b;
    return true;
}

/**
 * @brief 多样性表决 - 三取二
 * @param value_a 值A
 * @param value_b 值B
 * @param value_c 值C
 * @param result 表决结果
 * @return true: 表决成功, false: 无法表决
 */
bool Safety_Vote_2oo3(int32_t value_a, int32_t value_b,
                       int32_t value_c, int32_t *result) {
    if (NULL == result) {
        return false;
    }

    /* 三取二表决逻辑 */
    if (value_a == value_b) {
        *result = value_a;
        return true;
    }

    if (value_a == value_c) {
        *result = value_a;
        return true;
    }

    if (value_b == value_c) {
        *result = value_b;
        return true;
    }

    /* 三个值都不同 - 表决失败 */
    return false;
}

/**
 * @brief 安全内存比较
 * @note 恒定时间执行，防止时序攻击
 */
bool Safety_Memcmp_Volatile(const uint8_t *a, const uint8_t *b, uint32_t len) {
    volatile uint8_t result = 0U;
    uint32_t i;

    if ((NULL == a) || (NULL == b)) {
        return false;
    }

    /* 恒定时间比较 */
    for (i = 0U; i < len; i++) {
        result |= a[i] ^ b[i];
    }

    return (0U == result);
}

/**
 * @brief 范围限制函数
 */
int32_t Safety_Saturate_Int32(int32_t value, int32_t min_val, int32_t max_val) {
    if (value < min_val) {
        return min_val;
    }

    if (value > max_val) {
        return max_val;
    }

    return value;
}

/**
 * @brief CRC-32校验计算
 * @note 用于数据完整性验证
 */
uint32_t Safety_Calculate_CRC32(const uint8_t *data, uint32_t length) {
    uint32_t crc = 0xFFFFFFFFU;
    uint32_t i, j;

    if (NULL == data) {
        return 0U;
    }

    for (i = 0U; i < length; i++) {
        crc ^= (uint32_t)data[i];

        for (j = 0U; j < 8U; j++) {
            if (crc & 1U) {
                crc = (crc >> 1U) ^ 0xEDB88320U;
            } else {
                crc >>= 1U;
            }
        }
    }

    return ~crc;
}

/**
 * @brief 安全memcpy - 带长度验证
 */
bool Safety_Memcpy(uint8_t *dest, uint32_t dest_size,
                   const uint8_t *src, uint32_t src_size) {
    uint32_t copy_size;

    if ((NULL == dest) || (NULL == src)) {
        return false;
    }

    /* 确定复制大小 */
    copy_size = (dest_size < src_size) ? dest_size : src_size;

    /* 执行复制 */
    while (copy_size > 0U) {
        *dest++ = *src++;
        copy_size--;
    }

    return true;
}
