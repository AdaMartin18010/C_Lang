/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\Memory_Constrained\01_Memory_Constrained_Design.md
 * Line: 2145
 * Language: c
 * Block ID: ee0f6270
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ============================================================================
 * 位域使用指南 - 节省内存但需谨慎
 * ============================================================================ */

#include <stdint.h>
#include <stdbool.h>

/* 位域基本语法 */
typedef struct {
    unsigned int bit0 : 1;    /* 1位 */
    unsigned int bit1_2 : 2;  /* 2位 */
    unsigned int bit3_6 : 4;  /* 4位 */
    unsigned int bit7 : 1;    /* 1位 */
} byte_flags_t;  /* 总共8位 = 1字节 */

/* 位域打包 */
typedef struct {
    /* 第1字节 */
    unsigned int flag1 : 1;
    unsigned int flag2 : 1;
    unsigned int status : 2;
    unsigned int type : 4;

    /* 第2字节 */
    unsigned int error_code : 3;
    unsigned int reserved : 5;
} packed_flags_t;  /* 2字节 */

/* ============================================================================
 * 位域使用注意事项
 * ============================================================================ */

/* 警告1: 位域布局是编译器相关的 */
/* GCC: 从最低有效位开始填充
 * MSVC: 从最高有效位开始填充
 */

/* 警告2: 位域类型必须是 unsigned int, signed int, 或 _Bool */
/* 不推荐: 使用int (实现相关) */
typedef struct {
    int flag1 : 1;  /* 可能是有符号的! */
} bad_bitfield_t;

/* 推荐: 明确使用unsigned */
typedef struct {
    unsigned int flag1 : 1;  /* 明确无符号 */
} good_bitfield_t;

/* 警告3: 位域地址无法获取 */
typedef struct {
    unsigned int flag : 1;
} test_t;

test_t t;
/* void *p = &t.flag; */  /* 错误! 无法取地址 */

/* 警告4: 位域大小限制于类型 */
typedef struct {
    unsigned int a : 16;
    unsigned int b : 16;
    unsigned int c : 16;  /* 跨两个字，可能有填充 */
} large_bitfield_t;

/* 警告5: 对齐和填充 */
typedef struct {
    uint8_t  byte1;       /* 1字节 */
    /* 3字节填充! */
    unsigned int flags : 8;  /* 在下一个字 */
} misaligned_t;  /* 8字节而不是5字节 */

/* 修正 */
typedef struct {
    unsigned int flags : 8;  /* 打包在一起 */
    uint8_t  byte1;          /* 紧接着 */
} aligned_t;  /* 4字节 */

/* ============================================================================
 * 位域的最佳实践
 * ============================================================================ */

/* 实践1: 使用明确的整数类型 */
typedef struct __attribute__((packed)) {
    uint8_t  is_active : 1;
    uint8_t  has_error : 1;
    uint8_t  power_mode : 2;
    uint8_t  reserved : 4;
} device_status_t;

/* 实践2: 提供访问宏/函数 */
typedef struct {
    uint8_t bits;
} status_bits_t;

#define STATUS_IS_ACTIVE(s)    (((s).bits >> 0) & 1)
#define STATUS_HAS_ERROR(s)    (((s).bits >> 1) & 1)
#define STATUS_GET_POWER(s)    (((s).bits >> 2) & 3)

#define STATUS_SET_ACTIVE(s, v) \
    ((s).bits = ((s).bits & ~(1 << 0)) | (((v) & 1) << 0))
#define STATUS_SET_ERROR(s, v) \
    ((s).bits = ((s).bits & ~(1 << 1)) | (((v) & 1) << 1))
#define STATUS_SET_POWER(s, v) \
    ((s).bits = ((s).bits & ~(3 << 2)) | (((v) & 3) << 2))

/* 实践3: 位掩码替代位域 (更可控) */
typedef uint8_t status_mask_t;

#define MASK_ACTIVE   (1 << 0)
#define MASK_ERROR    (1 << 1)
#define MASK_POWER_L  (1 << 2)
#define MASK_POWER_H  (1 << 3)
#define MASK_POWER    (MASK_POWER_L | MASK_POWER_H)

static inline bool status_get_active(status_mask_t s)
{
    return (s & MASK_ACTIVE) != 0;
}

static inline void status_set_active(status_mask_t *s, bool val)
{
    if (val) *s |= MASK_ACTIVE;
    else *s &= ~MASK_ACTIVE;
}

static inline uint8_t status_get_power(status_mask_t s)
{
    return (s & MASK_POWER) >> 2;
}

static inline void status_set_power(status_mask_t *s, uint8_t val)
{
    *s = (*s & ~MASK_POWER) | ((val & 3) << 2);
}

/* 实践4: 联合体实现位域访问 */
typedef union {
    uint8_t byte;
    struct __attribute__((packed)) {
        uint8_t is_active : 1;
        uint8_t has_error : 1;
        uint8_t power_mode : 2;
        uint8_t reserved : 4;
    } bits;
} status_u_t;

/* 可以整体赋值或按位访问 */
status_u_t status;
status.byte = 0x05;  /* 整体设置 */
status.bits.is_active = 1;  /* 按位设置 */
