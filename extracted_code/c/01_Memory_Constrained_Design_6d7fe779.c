/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\Memory_Constrained\01_Memory_Constrained_Design.md
 * Line: 1958
 * Language: c
 * Block ID: 6d7fe779
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ============================================================================
 * 数据结构压缩 - 在不影响功能的前提下最小化内存占用
 * ============================================================================ */

#include <stdint.h>
#include <stdbool.h>

/* 原始结构体 - 16字节 */
typedef struct {
    uint32_t timestamp;   /* 4字节 */
    uint16_t value;       /* 2字节 */
    uint8_t  sensor_id;   /* 1字节 */
    uint8_t  status;      /* 1字节 */
    float    reading;     /* 4字节 */
    uint8_t  flags;       /* 1字节 + 3字节填充 */
} sensor_original_t;

/* 优化版本1: 重新排序 - 12字节 */
typedef struct {
    uint32_t timestamp;   /* 4字节 */
    float    reading;     /* 4字节 */
    uint16_t value;       /* 2字节 */
    uint8_t  sensor_id;   /* 1字节 */
    uint8_t  status;      /* 1字节 */
    uint8_t  flags;       /* 1字节 + 1字节填充 */
} sensor_reordered_t;

/* 优化版本2: 使用位域 - 10字节 */
typedef struct __attribute__((packed)) {
    uint32_t timestamp;      /* 4字节 */
    float    reading;        /* 4字节 */
    uint16_t value;          /* 2字节 */
    uint8_t  sensor_id;      /* 1字节 */
    uint8_t  status;         /* 1字节 */
} sensor_packed_t;

typedef struct __attribute__((packed)) {
    unsigned int is_active : 1;
    unsigned int has_error : 1;
    unsigned int reserved : 6;
} sensor_flags_t;

/* 优化版本3: 使用定点数替代浮点数 - 8字节 */
typedef struct __attribute__((packed)) {
    uint32_t timestamp;      /* 4字节 */
    int16_t  reading_fixed;  /* 2字节，0.01精度 */
    uint16_t value;          /* 2字节 */
    uint8_t  sensor_id;      /* 1字节 */
    uint8_t  status;         /* 1字节 */
} sensor_fixed_t;

/* 定点数转换宏 */
#define FLOAT_TO_FIXED(f)    ((int16_t)((f) * 100.0f))
#define FIXED_TO_FLOAT(fx)   ((float)(fx) / 100.0f)

/* 优化版本4: 时间戳压缩 - 6字节 */
typedef struct __attribute__((packed)) {
    uint16_t timestamp_delta;  /* 相对时间戳，2字节 */
    int16_t  reading_fixed;    /* 2字节 */
    uint8_t  sensor_id;        /* 1字节 */
    uint8_t  status;           /* 1字节 */
} sensor_compressed_t;

/* 时间戳管理 */
static uint32_t base_timestamp = 0;

void sensor_set_base_time(uint32_t timestamp)
{
    base_timestamp = timestamp;
}

void sensor_pack(sensor_compressed_t *out, uint32_t timestamp,
                 float reading, uint8_t id, uint8_t status)
{
    out->timestamp_delta = (uint16_t)(timestamp - base_timestamp);
    out->reading_fixed = FLOAT_TO_FIXED(reading);
    out->sensor_id = id;
    out->status = status;
}
