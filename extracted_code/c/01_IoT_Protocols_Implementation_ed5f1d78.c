/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\IoT_Protocols\01_IoT_Protocols_Implementation.md
 * Line: 3167
 * Language: c
 * Block ID: ed5f1d78
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file can_driver.h
 * @brief CAN总线驱动接口
 */

#ifndef CAN_DRIVER_H
#define CAN_DRIVER_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* CAN标准帧ID长度 */
#define CAN_STD_ID_BITS     11
#define CAN_EXT_ID_BITS     29
#define CAN_MAX_STD_ID      0x7FF
#define CAN_MAX_EXT_ID      0x1FFFFFFF

/* CAN帧数据长度 */
#define CAN_MAX_DLC         8
#define CAN_FD_MAX_DLC      64

/* CAN帧类型 */
typedef enum {
    CAN_FRAME_DATA = 0,         /* 数据帧 */
    CAN_FRAME_REMOTE = 1,       /* 远程帧 */
    CAN_FRAME_ERROR = 2,        /* 错误帧 */
    CAN_FRAME_OVERLOAD = 3      /* 过载帧 */
} can_frame_type_t;

/* CAN ID类型 */
typedef enum {
    CAN_ID_STANDARD = 0,        /* 标准帧 (11位ID) */
    CAN_ID_EXTENDED = 1         /* 扩展帧 (29位ID) */
} can_id_type_t;

/* CAN帧结构 */
typedef struct {
    uint32_t id;                /* 标识符 */
    can_id_type_t id_type;      /* ID类型 */
    can_frame_type_t frame_type; /* 帧类型 */
    uint8_t dlc;                /* 数据长度代码 (0-8) */
    uint8_t data[CAN_FD_MAX_DLC]; /* 数据 */
    bool rtr;                   /* 远程传输请求 */
    uint32_t timestamp;         /* 时间戳 */
} can_frame_t;

/* CAN位时序配置 */
typedef struct {
    uint32_t bitrate;           /* 波特率 (bps) */
    uint16_t prescaler;         /* 预分频器 */
    uint8_t time_seg1;          /* 时间段1 */
    uint8_t time_seg2;          /* 时间段2 */
    uint8_t sjw;                /* 同步跳转宽度 */
    bool triple_sample;         /* 三重采样 */
} can_bit_timing_t;

/* 滤波器配置 */
typedef struct {
    uint32_t id;                /* 滤波器ID */
    uint32_t mask;              /* 掩码 */
    can_id_type_t id_type;      /* ID类型 */
    bool enabled;               /* 使能 */
} can_filter_t;

/* 错误码 */
typedef enum {
    CAN_ERR_OK = 0,
    CAN_ERR_INVALID_PARAM = -1,
    CAN_ERR_BUSY = -2,
    CAN_ERR_TIMEOUT = -3,
    CAN_ERR_BUS_OFF = -4,
    CAN_ERR_PASSIVE = -5,
    CAN_ERR_OVERRUN = -6,
    CAN_ERR_NO_MEMORY = -7
} can_error_t;

/* 总线状态 */
typedef enum {
    CAN_BUS_ERROR_ACTIVE = 0,   /* 主动错误 */
    CAN_BUS_ERROR_PASSIVE = 1,  /* 被动错误 */
    CAN_BUS_OFF = 2             /* 总线关闭 */
} can_bus_state_t;

/* 错误计数器结构 */
typedef struct {
    uint8_t tx_error_count;     /* 发送错误计数 */
    uint8_t rx_error_count;     /* 接收错误计数 */
    can_bus_state_t bus_state;  /* 总线状态 */
} can_error_counters_t;

/* 回调函数类型 */
typedef void (*can_rx_callback_t)(const can_frame_t *frame, void *user_data);
typedef void (*can_error_callback_t)(can_error_counters_t *counters, void *user_data);

/* CAN设备句柄 */
typedef struct can_device can_device_t;

/* API函数 */

/* 初始化和关闭 */
can_device_t* can_init(uint8_t controller_id, const can_bit_timing_t *timing);
void can_deinit(can_device_t *dev);

/* 配置 */
int can_set_bit_timing(can_device_t *dev, const can_bit_timing_t *timing);
int can_set_filter(can_device_t *dev, uint8_t filter_idx, const can_filter_t *filter);
int can_clear_filter(can_device_t *dev, uint8_t filter_idx);

/* 发送和接收 */
int can_send(can_device_t *dev, const can_frame_t *frame, uint32_t timeout_ms);
int can_receive(can_device_t *dev, can_frame_t *frame, uint32_t timeout_ms);
int can_try_receive(can_device_t *dev, can_frame_t *frame);

/* 中断和回调 */
int can_enable_interrupt(can_device_t *dev);
int can_disable_interrupt(can_device_t *dev);
int can_register_rx_callback(can_device_t *dev, can_rx_callback_t callback, void *user_data);
int can_register_error_callback(can_device_t *dev, can_error_callback_t callback, void *user_data);

/* 状态和控制 */
int can_start(can_device_t *dev);
int can_stop(can_device_t *dev);
int can_get_error_counters(can_device_t *dev, can_error_counters_t *counters);
int can_reset(can_device_t *dev);
bool can_is_bus_off(can_device_t *dev);

/* 实用函数 */
uint16_t can_dlc_to_len(uint8_t dlc);
uint8_t can_len_to_dlc(uint16_t len);
bool can_is_valid_id(uint32_t id, can_id_type_t type);
void can_frame_init(can_frame_t *frame, uint32_t id, can_id_type_t id_type,
                     const uint8_t *data, uint8_t len);

/* 常用波特率配置 */
extern const can_bit_timing_t can_timing_125k;
extern const can_bit_timing_t can_timing_250k;
extern const can_bit_timing_t can_timing_500k;
extern const can_bit_timing_t can_timing_1m;

#ifdef __cplusplus
}
#endif

#endif /* CAN_DRIVER_H */
