/*
 * Auto-generated from: 04_Industrial_Scenarios\02_Avionics_Systems\01_ARINC_429.md
 * Line: 229
 * Language: c
 * Block ID: 83f3eb99
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file arinc429.h
 * @brief ARINC 429协议C语言实现头文件
 * @version 1.0.0
 */

#ifndef ARINC429_H
#define ARINC429_H

#include <stdint.h>
#include <stdbool.h>

/* ARINC 429字定义 */
typedef uint32_t ARINC429_Word_t;

/* 数据速率定义 */
typedef enum {
    ARINC429_SPEED_LOW = 0,     /* 12.5 kbps */
    ARINC429_SPEED_HIGH = 1     /* 100 kbps */
} ARINC429_Speed_t;

/* 数据格式类型 */
typedef enum {
    ARINC429_FMT_BNR = 0,       /* 二进制数值表示 */
    ARINC429_FMT_BCD = 1,       /* BCD编码 */
    ARINC429_FMT_DISCRETE = 2,  /* 离散数据 */
    ARINC429_FMT_MAINTENANCE = 3 /* 维护数据 */
} ARINC429_Format_t;

/* SSM定义 (BNR格式) */
typedef enum {
    ARINC429_SSM_PLUS = 0,      /* 正方向 */
    ARINC429_SSM_MINUS = 1,     /* 负方向 */
    ARINC429_SSM_ALTERNATE = 2, /* 备用 */
    ARINC429_SSM_INVALID = 3    /* 无效/故障 */
} ARINC429_SSM_t;

/* 标签定义 (八进制) */
#define ARINC429_LABEL_STATIC_PRESSURE  0010U
#define ARINC429_LABEL_CAL_AIRSPEED     0025U
#define ARINC429_LABEL_BARO_ALTITUDE    0057U
#define ARINC429_LABEL_MAG_HEADING      0106U
#define ARINC429_LABEL_SEL_ALTITUDE     0204U
#define ARINC429_LABEL_TIME             0260U
#define ARINC429_LABEL_DATE             0270U
#define ARINC429_LABEL_WP_LATITUDE      0310U
#define ARINC429_LABEL_WP_LONGITUDE     0311U

/* 字段位掩码 */
#define ARINC429_MASK_LABEL     (0xFF000000U)
#define ARINC429_MASK_SDI       (0x00C00000U)
#define ARINC429_MASK_DATA      (0x003FFFF8U)
#define ARINC429_MASK_SSM       (0x00000006U)
#define ARINC429_MASK_PARITY    (0x00000001U)

/* 字段位移 */
#define ARINC429_SHIFT_LABEL    (24U)
#define ARINC429_SHIFT_SDI      (22U)
#define ARINC429_SHIFT_DATA     (3U)
#define ARINC429_SHIFT_SSM      (1U)

/* 函数声明 */
ARINC429_Word_t ARINC429_EncodeWord(uint8_t label, uint8_t sdi,
                                     uint32_t data, uint8_t ssm);
bool ARINC429_DecodeWord(ARINC429_Word_t word, uint8_t *label,
                          uint8_t *sdi, uint32_t *data, uint8_t *ssm);
bool ARINC429_ValidateParity(ARINC429_Word_t word);
ARINC429_Word_t ARINC429_SetParity(ARINC429_Word_t word);

#endif /* ARINC429_H */
