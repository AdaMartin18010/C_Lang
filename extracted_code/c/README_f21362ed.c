/*
 * Auto-generated from: 04_Industrial_Scenarios\02_Avionics_Systems\README.md
 * Line: 1162
 * Language: c
 * Block ID: f21362ed
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file ttp_schedule.h
 * @brief TTP/C 时间触发协议调度表定义
 *
 * TTP/C（Time-Triggered Protocol）用于确定性的分布式实时系统
 */

#ifndef TTP_SCHEDULE_H
#define TTP_SCHEDULE_H

#include <stdint.h>

/* TTP/C 参数定义 */
#define TTP_NODE_COUNT      4       /* 网络节点数量 */
#define TTP_SLOT_COUNT      16      /* TDMA轮询中的时隙数 */
#define TTP_CYCLE_TIME_US   2500    /* TDMA轮询周期：2.5ms */
#define TTP_SLOT_TIME_US    156     /* 每个时隙：156.25us */

/**
 * @brief 消息描述符（ MEDL - Message Descriptor List）
 *
 * 定义每个时隙的传输属性
 */
typedef struct {
    uint8_t node_id;            /* 发送节点ID */
    uint16_t frame_id;          /* 帧标识 */
    uint8_t data_length;        /* 数据长度（字节） */
    uint16_t offset_us;         /* 时隙内偏移（微秒） */
    uint8_t flags;              /* 标志位 */
    #define TTP_FLAG_TX     0x01    /* 发送时隙 */
    #define TTP_FLAG_RX     0x02    /* 接收时隙 */
    #define TTP_FLAG_CR     0x04    /* 时钟校正时隙 */
    #define TTP_FLAG_CS     0x08    /* 成员服务时隙 */
} TtpSlotDescriptor;

/**
 * @brief TTP/C MEDL 调度表
 *
 * 这个调度表定义了4个节点的TDMA轮询
 * 每个节点每轮询周期获得4个时隙
 */
static const TtpSlotDescriptor g_ttp_schedule[TTP_SLOT_COUNT] = {
    /* 时隙0-3: 节点0的时隙 */
    {0, 0x100, 16, 0,   TTP_FLAG_TX | TTP_FLAG_CR},  /* 节点0发送，时钟校正 */
    {0, 0x101, 16, 0,   TTP_FLAG_TX},
    {0, 0x102, 16, 0,   TTP_FLAG_TX},
    {0, 0x103, 16, 0,   TTP_FLAG_TX | TTP_FLAG_CS},  /* 成员服务 */

    /* 时隙4-7: 节点1的时隙 */
    {1, 0x200, 16, 0,   TTP_FLAG_TX | TTP_FLAG_CR},
    {1, 0x201, 16, 0,   TTP_FLAG_TX},
    {1, 0x202, 16, 0,   TTP_FLAG_TX},
    {1, 0x203, 16, 0,   TTP_FLAG_TX},

    /* 时隙8-11: 节点2的时隙 */
    {2, 0x300, 16, 0,   TTP_FLAG_TX | TTP_FLAG_CR},
    {2, 0x301, 16, 0,   TTP_FLAG_TX},
    {2, 0x302, 16, 0,   TTP_FLAG_TX},
    {2, 0x303, 16, 0,   TTP_FLAG_TX},

    /* 时隙12-15: 节点3的时隙 */
    {3, 0x400, 16, 0,   TTP_FLAG_TX | TTP_FLAG_CR},
    {3, 0x401, 16, 0,   TTP_FLAG_TX},
    {3, 0x402, 16, 0,   TTP_FLAG_TX},
    {3, 0x403, 16, 0,   TTP_FLAG_TX},
};

/**
 * @brief TTP/C 帧格式
 */
typedef struct __attribute__((packed)) {
    uint8_t header;             /* 帧头 */
    uint16_t frame_id;          /* 帧标识 */
    uint8_t node_id;            /* 发送节点ID */
    uint32_t global_time;       /* 全局时间戳 */
    uint8_t data[16];           /* 有效载荷 */
    uint16_t crc;               /* CRC校验 */
} TtpFrame;

/**
 * @brief 时钟同步参数
 */
typedef struct {
    int32_t clock_correction;   /* 时钟校正值（微秒） */
    uint32_t local_time;        /* 本地时间 */
    uint32_t global_time;       /* 全局时间 */
    uint8_t sync_quality;       /* 同步质量 */
} TtpClockSync;

#endif /* TTP_SCHEDULE_H */
