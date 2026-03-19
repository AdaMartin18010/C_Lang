/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\IoT_Protocols\01_IoT_Protocols_Implementation.md
 * Line: 2436
 * Language: c
 * Block ID: 5aa2d274
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file modbus.h
 * @brief Modbus RTU/TCP协议实现
 */

#ifndef MODBUS_H
#define MODBUS_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Modbus模式 */
typedef enum {
    MODBUS_RTU,     /* 串行RTU模式 */
    MODBUS_ASCII,   /* 串行ASCII模式 */
    MODBUS_TCP      /* TCP模式 */
} modbus_mode_t;

/* 功能码 */
typedef enum {
    MODBUS_FC_READ_COILS = 0x01,
    MODBUS_FC_READ_DISCRETE_INPUTS = 0x02,
    MODBUS_FC_READ_HOLDING_REGISTERS = 0x03,
    MODBUS_FC_READ_INPUT_REGISTERS = 0x04,
    MODBUS_FC_WRITE_SINGLE_COIL = 0x05,
    MODBUS_FC_WRITE_SINGLE_REGISTER = 0x06,
    MODBUS_FC_READ_EXCEPTION_STATUS = 0x07,
    MODBUS_FC_DIAGNOSTICS = 0x08,
    MODBUS_FC_GET_COMM_EVENT_COUNTER = 0x0B,
    MODBUS_FC_GET_COMM_EVENT_LOG = 0x0C,
    MODBUS_FC_WRITE_MULTIPLE_COILS = 0x0F,
    MODBUS_FC_WRITE_MULTIPLE_REGISTERS = 0x10,
    MODBUS_FC_REPORT_SLAVE_ID = 0x11,
    MODBUS_FC_READ_FILE_RECORD = 0x14,
    MODBUS_FC_WRITE_FILE_RECORD = 0x15,
    MODBUS_FC_MASK_WRITE_REGISTER = 0x16,
    MODBUS_FC_READ_WRITE_MULTIPLE_REGISTERS = 0x17,
    MODBUS_FC_READ_FIFO_QUEUE = 0x18,
    MODBUS_FC_ENCAPSULATED_INTERFACE = 0x2B
} modbus_function_code_t;

/* 异常码 */
typedef enum {
    MODBUS_EX_NONE = 0x00,
    MODBUS_EX_ILLEGAL_FUNCTION = 0x01,
    MODBUS_EX_ILLEGAL_DATA_ADDRESS = 0x02,
    MODBUS_EX_ILLEGAL_DATA_VALUE = 0x03,
    MODBUS_EX_SLAVE_DEVICE_FAILURE = 0x04,
    MODBUS_EX_ACKNOWLEDGE = 0x05,
    MODBUS_EX_SLAVE_DEVICE_BUSY = 0x06,
    MODBUS_EX_NEGATIVE_ACKNOWLEDGE = 0x07,
    MODBUS_EX_MEMORY_PARITY_ERROR = 0x08,
    MODBUS_EX_GATEWAY_PATH_UNAVAILABLE = 0x0A,
    MODBUS_EX_GATEWAY_TARGET_FAILED = 0x0B
} modbus_exception_code_t;

/* 错误码 */
typedef enum {
    MODBUS_ERR_OK = 0,
    MODBUS_ERR_INVALID_PARAM = -1,
    MODBUS_ERR_TIMEOUT = -2,
    MODBUS_ERR_CRC = -3,
    MODBUS_ERR_EXCEPTION = -4,
    MODBUS_ERR_SEND = -5,
    MODBUS_ERR_RECV = -6,
    MODBUS_ERR_INVALID_SLAVE = -7,
    MODBUS_ERR_INVALID_FUNCTION = -8
} modbus_error_t;

/* 最大配置 */
#define MODBUS_MAX_ADU_SIZE         256
#define MODBUS_MAX_PDU_SIZE         253
#define MODBUS_MAX_READ_BITS        2000
#define MODBUS_MAX_WRITE_BITS       1968
#define MODBUS_MAX_READ_REGISTERS   125
#define MODBUS_MAX_WRITE_REGISTERS  123
#define MODBUS_MAX_WR_READ_REGISTERS 125
#define MODBUS_MAX_WR_WRITE_REGISTERS 121

/* TCP MBAP头长度 */
#define MODBUS_TCP_MBAP_SIZE        7

/* 默认端口 */
#define MODBUS_TCP_DEFAULT_PORT     502

/* 前向声明 */
struct modbus_context;
typedef struct modbus_context modbus_context_t;

/* 传输层回调 */
typedef int (*modbus_send_t)(void *ctx, const uint8_t *data, size_t len);
typedef int (*modbus_recv_t)(void *ctx, uint8_t *data, size_t max_len, uint32_t timeout_ms);
typedef void (*modbus_flush_t)(void *ctx);

/* Modbus上下文 */
struct modbus_context {
    modbus_mode_t mode;
    uint8_t slave_id;

    /* 传输层 */
    void *transport_ctx;
    modbus_send_t send;
    modbus_recv_t recv;
    modbus_flush_t flush;

    /* 超时配置 */
    uint32_t response_timeout_ms;
    uint32_t byte_timeout_ms;

    /* TCP特定 */
    uint16_t transaction_id;

    /* 缓冲区 */
    uint8_t tx_buffer[MODBUS_MAX_ADU_SIZE];
    uint8_t rx_buffer[MODBUS_MAX_ADU_SIZE];

    /* 统计 */
    uint32_t tx_count;
    uint32_t rx_count;
    uint32_t error_count;
    uint32_t timeout_count;
};

/* 初始化 */
int modbus_init_rtu(modbus_context_t *ctx, void *transport_ctx,
                     modbus_send_t send_func, modbus_recv_t recv_func,
                     modbus_flush_t flush_func, uint8_t slave_id);
int modbus_init_tcp(modbus_context_t *ctx, void *transport_ctx,
                     modbus_send_t send_func, modbus_recv_t recv_func);

/* 主站操作 */
int modbus_read_bits(modbus_context_t *ctx, uint8_t slave_id, uint16_t addr,
                      uint16_t count, uint8_t *values);
int modbus_read_input_bits(modbus_context_t *ctx, uint8_t slave_id, uint16_t addr,
                            uint16_t count, uint8_t *values);
int modbus_read_registers(modbus_context_t *ctx, uint8_t slave_id, uint16_t addr,
                           uint16_t count, uint16_t *values);
int modbus_read_input_registers(modbus_context_t *ctx, uint8_t slave_id, uint16_t addr,
                                 uint16_t count, uint16_t *values);
int modbus_write_bit(modbus_context_t *ctx, uint8_t slave_id, uint16_t addr, bool value);
int modbus_write_register(modbus_context_t *ctx, uint8_t slave_id, uint16_t addr,
                           uint16_t value);
int modbus_write_bits(modbus_context_t *ctx, uint8_t slave_id, uint16_t addr,
                       uint16_t count, const uint8_t *values);
int modbus_write_registers(modbus_context_t *ctx, uint8_t slave_id, uint16_t addr,
                            uint16_t count, const uint16_t *values);
int modbus_mask_write_register(modbus_context_t *ctx, uint8_t slave_id, uint16_t addr,
                                uint16_t and_mask, uint16_t or_mask);
int modbus_write_and_read_registers(modbus_context_t *ctx, uint8_t slave_id,
                                     uint16_t write_addr, uint16_t write_count,
                                     const uint16_t *write_values,
                                     uint16_t read_addr, uint16_t read_count,
                                     uint16_t *read_values);

/* 从站操作(简化实现) */
int modbus_slave_poll(modbus_context_t *ctx, uint8_t *request, size_t *req_len,
                       uint32_t timeout_ms);
int modbus_slave_reply(modbus_context_t *ctx, const uint8_t *request,
                        const uint8_t *response, size_t resp_len);
int modbus_reply_exception(modbus_context_t *ctx, const uint8_t *request,
                            modbus_exception_code_t exception);

/* 工具函数 */
uint16_t modbus_crc16(const uint8_t *data, size_t len);
bool modbus_check_crc16(const uint8_t *data, size_t len);
const char* modbus_strerror(modbus_error_t err);
const char* modbus_exception_str(modbus_exception_code_t exc);

/* 数据转换 */
static inline uint16_t modbus_get_u16(const uint8_t *data) {
    return (uint16_t)((data[0] << 8) | data[1]);
}

static inline void modbus_set_u16(uint8_t *data, uint16_t value) {
    data[0] = (uint8_t)(value >> 8);
    data[1] = (uint8_t)(value & 0xFF);
}

static inline uint32_t modbus_get_u32(const uint8_t *data) {
    return ((uint32_t)data[0] << 24) |
           ((uint32_t)data[1] << 16) |
           ((uint32_t)data[2] << 8) |
           (uint32_t)data[3];
}

static inline void modbus_set_u32(uint8_t *data, uint32_t value) {
    data[0] = (uint8_t)(value >> 24);
    data[1] = (uint8_t)(value >> 16);
    data[2] = (uint8_t)(value >> 8);
    data[3] = (uint8_t)(value);
}

static inline float modbus_get_float(const uint8_t *data) {
    union { uint32_t u; float f; } conv;
    conv.u = modbus_get_u32(data);
    return conv.f;
}

static inline void modbus_set_float(uint8_t *data, float value) {
    union { uint32_t u; float f; } conv;
    conv.f = value;
    modbus_set_u32(data, conv.u);
}

#ifdef __cplusplus
}
#endif

#endif /* MODBUS_H */
