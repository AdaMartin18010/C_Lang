/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\Memory_Constrained\01_Memory_Constrained_Design.md
 * Line: 1463
 * Language: c
 * Block ID: 34e2cfd8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ============================================================================
 * 内存对齐与填充 - 理解编译器的内存布局
 * ============================================================================ */

/* 结构体填充示例 */
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

/* 未优化的结构体 - 浪费空间 */
typedef struct {
    uint8_t  flag;        /* 1字节 + 3字节填充 */
    uint32_t timestamp;   /* 4字节 */
    uint8_t  status;      /* 1字节 + 1字节填充 */
    uint16_t value;       /* 2字节 */
} bad_layout_t;
/* 总计: 12字节 (浪费4字节填充) */

/* 优化后的结构体 - 按大小排序 */
typedef struct {
    uint32_t timestamp;   /* 4字节 */
    uint16_t value;       /* 2字节 */
    uint8_t  flag;        /* 1字节 */
    uint8_t  status;      /* 1字节 */
} good_layout_t;
/* 总计: 8字节 (无浪费) */

/* 编译器特定打包 */
/* GCC */
typedef struct __attribute__((packed)) {
    uint8_t  flag;
    uint32_t timestamp;   /* 未对齐访问可能慢 */
    uint8_t  status;
    uint16_t value;
} packed_gcc_t;
/* 总计: 8字节，但可能有性能损失 */

/* MSVC */
#pragma pack(push, 1)
typedef struct {
    uint8_t  flag;
    uint32_t timestamp;
    uint8_t  status;
    uint16_t value;
} packed_msvc_t;
#pragma pack(pop)

/* 验证大小 */
void check_sizes(void)
{
    printf("bad_layout_t:  %zu bytes\n", sizeof(bad_layout_t));
    printf("good_layout_t: %zu bytes\n", sizeof(good_layout_t));
    printf("packed_gcc_t:  %zu bytes\n", sizeof(packed_gcc_t));
}

/* 数组对齐优化 */
/* 32位DMA通常需要4字节对齐 */
struct {
    uint8_t small_data[10];   /* 10字节 */
    /* 编译器插入2字节填充 */
    uint32_t dma_buffer[16];  /* 64字节，4字节对齐 */
} aligned_example;

/* 显式对齐 */
__attribute__((aligned(4))) uint8_t dma_buffer[64];
__attribute__((aligned(8))) uint64_t cache_line_buffer[8];

/* 混合数据类型的优化布局 */
typedef struct {
    /* 第一组: 4字节对齐 */
    uint32_t id;
    uint32_t timestamp;

    /* 第二组: 2字节对齐 */
    uint16_t x;
    uint16_t y;
    uint16_t z;

    /* 第三组: 1字节 */
    uint8_t flags;
    uint8_t status;
    uint8_t reserved[2];  /* 填充到4字节边界 */
} sensor_optimized_t;
/* 总计: 16字节，无填充浪费 */

/* 位域与成员的混合使用 */
typedef struct {
    /* 位域打包到一个字节 */
    uint8_t is_active : 1;
    uint8_t has_error : 1;
    uint8_t power_mode : 2;
    uint8_t reserved : 4;

    /* 完整字节 */
    uint8_t battery_level;

    /* 2字节对齐 */
    uint16_t sample_rate;

    /* 4字节对齐 */
    uint32_t total_samples;
} mixed_optimized_t;
/* 总计: 8字节 */
