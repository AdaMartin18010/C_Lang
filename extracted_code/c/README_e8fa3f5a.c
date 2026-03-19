/*
 * Auto-generated from: 04_Industrial_Scenarios\01_Automotive_ABS\README.md
 * Line: 131
 * Language: c
 * Block ID: e8fa3f5a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* network/can_driver.h */
#ifndef CAN_DRIVER_H
#define CAN_DRIVER_H

#include <stdint.h>

/* CAN ID 定义 - 标准帧 11 位 */
#define CAN_ID_ABS_WHEEL_SPEED_FL   0x100  /* 左前轮速度 */
#define CAN_ID_ABS_WHEEL_SPEED_FR   0x101  /* 右前轮速度 */
#define CAN_ID_ABS_WHEEL_SPEED_RL   0x102  /* 左后轮速度 */
#define CAN_ID_ABS_WHEEL_SPEED_RR   0x103  /* 右后轮速度 */
#define CAN_ID_ABS_STATUS           0x200  /* ABS 状态 */
#define CAN_ID_BRAKE_PEDAL_POS      0x300  /* 制动踏板位置 */
#define CAN_ID_VEHICLE_SPEED        0x400  /* 车速 */

/* CAN 帧结构 */
typedef struct {
    uint32_t id;           /* 11 位或 29 位 ID */
    bool extended;         /* 是否为扩展帧 */
    bool rtr;              /* 远程传输请求 */
    uint8_t dlc;           /* 数据长度代码 (0-8) */
    uint8_t data[8];       /* 数据字节 */
    uint32_t timestamp_us; /* 接收时间戳 */
} can_frame_t;

/* CAN 回调函数类型 */
typedef void (*can_rx_callback_t)(const can_frame_t* frame, void* user_data);

typedef struct can_controller can_controller_t;

can_controller_t* can_init(uint8_t channel, uint32_t baudrate);
void can_register_rx_callback(can_controller_t* can,
                              uint32_t id,
                              uint32_t mask,
                              can_rx_callback_t callback,
                              void* user_data);
bool can_send(can_controller_t* can, const can_frame_t* frame, uint32_t timeout_ms);
void can_process_rx(can_controller_t* can);

/* 打包/解包辅助函数 */
static inline void pack_u16(uint8_t* data, uint16_t value) {
    data[0] = (uint8_t)(value & 0xFF);
    data[1] = (uint8_t)((value >> 8) & 0xFF);
}

static inline uint16_t unpack_u16(const uint8_t* data) {
    return (uint16_t)data[0] | ((uint16_t)data[1] << 8);
}

#endif
