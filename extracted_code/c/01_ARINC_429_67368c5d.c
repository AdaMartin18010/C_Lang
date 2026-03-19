/*
 * Auto-generated from: 04_Industrial_Scenarios\02_Avionics_Systems\01_ARINC_429.md
 * Line: 304
 * Language: c
 * Block ID: 67368c5d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file arinc429.c
 * @brief ARINC 429协议C语言实现
 */

#include "arinc429.h"

/**
 * @brief 反转8位数据 (用于标签编码)
 * @note ARINC 429标签使用LSB优先传输
 */
static uint8_t reverse_bits(uint8_t value) {
    uint8_t result = 0U;
    uint8_t i;

    for (i = 0U; i < 8U; i++) {
        result <<= 1U;
        result |= (value & 1U);
        value >>= 1U;
    }

    return result;
}

/**
 * @brief 计算奇偶校验位
 * @param word 31位数据字
 * @return 奇校验位 (使总1的个数为奇数)
 */
static uint8_t calculate_parity(uint32_t word) {
    uint8_t parity = 0U;
    uint8_t i;

    /* 计算32位中1的个数 */
    for (i = 0U; i < 32U; i++) {
        parity ^= (uint8_t)((word >> i) & 1U);
    }

    /* 奇校验: 若已有偶数个1, 返回1; 奇数个1, 返回0 */
    return (parity ^ 1U);
}

/**
 * @brief 编码ARINC 429字
 * @param label 8位标签 (八进制编码，会自动反转)
 * @param sdi 2位源/目标标识符
 * @param data 19位数据字段
 * @param ssm 2位符号/状态矩阵
 * @return 32位ARINC 429字
 */
ARINC429_Word_t ARINC429_EncodeWord(uint8_t label, uint8_t sdi,
                                     uint32_t data, uint8_t ssm) {
    ARINC429_Word_t word = 0U;
    uint8_t reversed_label;

    /* 反转标签位 (八进制LSB优先) */
    reversed_label = reverse_bits(label);

    /* 组装各字段 */
    word |= ((uint32_t)reversed_label << ARINC429_SHIFT_LABEL);
    word |= ((uint32_t)(sdi & 0x03U) << ARINC429_SHIFT_SDI);
    word |= ((data & 0x7FFFFU) << ARINC429_SHIFT_DATA);
    word |= ((uint32_t)(ssm & 0x03U) << ARINC429_SHIFT_SSM);

    /* 计算并设置奇偶校验位 */
    word = ARINC429_SetParity(word);

    return word;
}

/**
 * @brief 解码ARINC 429字
 * @param word 32位ARINC 429字
 * @param label 输出标签
 * @param sdi 输出SDI
 * @param data 输出数据字段
 * @param ssm 输出SSM
 * @return true: 成功, false: 校验失败
 */
bool ARINC429_DecodeWord(ARINC429_Word_t word, uint8_t *label,
                          uint8_t *sdi, uint32_t *data, uint8_t *ssm) {
    /* 校验奇偶校验位 */
    if (!ARINC429_ValidateParity(word)) {
        return false;
    }

    /* 提取并反转标签 */
    if (NULL != label) {
        uint8_t raw_label = (uint8_t)((word & ARINC429_MASK_LABEL)
                                      >> ARINC429_SHIFT_LABEL);
        *label = reverse_bits(raw_label);
    }

    /* 提取SDI */
    if (NULL != sdi) {
        *sdi = (uint8_t)((word & ARINC429_MASK_SDI) >> ARINC429_SHIFT_SDI);
    }

    /* 提取数据字段 */
    if (NULL != data) {
        *data = (word & ARINC429_MASK_DATA) >> ARINC429_SHIFT_DATA;
    }

    /* 提取SSM */
    if (NULL != ssm) {
        *ssm = (uint8_t)((word & ARINC429_MASK_SSM) >> ARINC429_SHIFT_SSM);
    }

    return true;
}

/**
 * @brief 验证奇偶校验
 * @param word 32位ARINC 429字
 * @return true: 校验通过, false: 校验失败
 */
bool ARINC429_ValidateParity(ARINC429_Word_t word) {
    uint8_t ones_count = 0U;
    uint8_t i;

    for (i = 0U; i < 32U; i++) {
        ones_count += (uint8_t)((word >> i) & 1U);
    }

    /* 奇校验: 1的总数应为奇数 */
    return ((ones_count & 1U) == 1U);
}

/**
 * @brief 设置奇偶校验位
 * @param word 31位数据字
 * @return 带校验位的32位字
 */
ARINC429_Word_t ARINC429_SetParity(ARINC429_Word_t word) {
    uint8_t parity;

    /* 清除现有校验位 */
    word &= ~ARINC429_MASK_PARITY;

    /* 计算并设置新校验位 */
    parity = calculate_parity(word);
    word |= (uint32_t)parity;

    return word;
}

/**
 * @brief BNR格式编码
 * @param value 浮点数值
 * @param resolution 分辨率 (LSB值)
 * @param num_bits 数据位数
 * @return 编码后的数据字段
 */
uint32_t ARINC429_Encode_BNR(double value, double resolution,
                              uint8_t num_bits, bool is_signed) {
    double scaled;
    uint32_t data = 0U;
    uint32_t max_value;

    if (is_signed) {
        /* 有符号数: 1位符号 + (num_bits-1)位数值 */
        max_value = (1U << (num_bits - 1U)) - 1U;

        /* 限制范围 */
        if (value > 0) {
            if ((value / resolution) > (double)max_value) {
                scaled = (double)max_value;
            } else {
                scaled = value / resolution;
            }
            data = (uint32_t)scaled;
        } else {
            if ((value / resolution) < -(double)max_value) {
                scaled = -(double)max_value;
            } else {
                scaled = value / resolution;
            }
            /* 符号位在Bit 21 */
            data = (1U << (num_bits - 1U)) | ((uint32_t)(-scaled) & (max_value));
        }
    } else {
        /* 无符号数 */
        max_value = (1U << num_bits) - 1U;

        if (value < 0) {
            value = 0;
        }

        scaled = value / resolution;
        if (scaled > (double)max_value) {
            scaled = (double)max_value;
        }

        data = (uint32_t)scaled;
    }

    return data;
}

/**
 * @brief BNR格式解码
 * @param data 数据字段
 * @param resolution 分辨率
 * @param num_bits 数据位数
 * @param is_signed 是否为有符号数
 * @return 解码后的浮点数值
 */
double ARINC429_Decode_BNR(uint32_t data, double resolution,
                            uint8_t num_bits, bool is_signed) {
    double value;
    uint32_t sign_bit;
    uint32_t value_mask;

    if (is_signed) {
        sign_bit = 1U << (num_bits - 1U);
        value_mask = sign_bit - 1U;

        if (data & sign_bit) {
            /* 负数 */
            value = -(double)(data & value_mask) * resolution;
        } else {
            /* 正数 */
            value = (double)(data & value_mask) * resolution;
        }
    } else {
        value = (double)data * resolution;
    }

    return value;
}
