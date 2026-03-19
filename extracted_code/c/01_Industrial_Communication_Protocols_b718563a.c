/*
 * Auto-generated from: 04_Industrial_Scenarios\15_Industrial_Communication\01_Industrial_Communication_Protocols.md
 * Line: 841
 * Language: c
 * Block ID: b718563a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file modbus_rtu.h
 * @brief Modbus RTU协议栈头文件
 * @version 2.0.0
 * @date 2024
 */

#ifndef MODBUS_RTU_H
#define MODBUS_RTU_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Modbus常量定义 */
#define MODBUS_BROADCAST_ADDRESS    0
#define MODBUS_MIN_SLAVE_ADDRESS    1
#define MODBUS_MAX_SLAVE_ADDRESS    247

#define MODBUS_RTU_MAX_ADU_LENGTH   256
#define MODBUS_RTU_MAX_PDU_LENGTH   253

/* 功能码定义 */
typedef enum {
    MODBUS_FC_READ_COILS                = 0x01,
    MODBUS_FC_READ_DISCRETE_INPUTS      = 0x02,
    MODBUS_FC_READ_HOLDING_REGISTERS    = 0x03,
    MODBUS_FC_READ_INPUT_REGISTERS      = 0x04,
    MODBUS_FC_WRITE_SINGLE_COIL         = 0x05,
    MODBUS_FC_WRITE_SINGLE_REGISTER     = 0x06,
    MODBUS_FC_READ_EXCEPTION_STATUS     = 0x07,
    MODBUS_FC_DIAGNOSTICS               = 0x08,
    MODBUS_FC_GET_COMM_EVENT_COUNTER    = 0x0B,
    MODBUS_FC_GET_COMM_EVENT_LOG        = 0x0C,
    MODBUS_FC_WRITE_MULTIPLE_COILS      = 0x0F,
    MODBUS_FC_WRITE_MULTIPLE_REGISTERS  = 0x10,
    MODBUS_FC_REPORT_SLAVE_ID           = 0x11,
    MODBUS_FC_READ_FILE_RECORD          = 0x14,
    MODBUS_FC_WRITE_FILE_RECORD         = 0x15,
    MODBUS_FC_MASK_WRITE_REGISTER       = 0x16,
    MODBUS_FC_READ_WRITE_MULTIPLE_REGS  = 0x17,
    MODBUS_FC_READ_FIFO_QUEUE           = 0x18,
    MODBUS_FC_READ_DEVICE_IDENTIFICATION= 0x2B
} modbus_function_code_t;

/* 异常码定义 */
typedef enum {
    MODBUS_EX_NONE                      = 0x00,
    MODBUS_EX_ILLEGAL_FUNCTION          = 0x01,
    MODBUS_EX_ILLEGAL_DATA_ADDRESS      = 0x02,
    MODBUS_EX_ILLEGAL_DATA_VALUE        = 0x03,
    MODBUS_EX_SLAVE_DEVICE_FAILURE      = 0x04,
    MODBUS_EX_ACKNOWLEDGE               = 0x05,
    MODBUS_EX_SLAVE_BUSY                = 0x06,
    MODBUS_EX_MEMORY_PARITY_ERROR       = 0x08,
    MODBUS_EX_GATEWAY_PATH_UNAVAILABLE  = 0x0A,
    MODBUS_EX_GATEWAY_TARGET_FAILED     = 0x0B
} modbus_exception_code_t;

/* 错误码定义 */
typedef enum {
    MODBUS_OK                   = 0,
    MODBUS_ERR_INVALID_PARAM    = -1,
    MODBUS_ERR_INVALID_SLAVE    = -2,
    MODBUS_ERR_INVALID_FUNCTION = -3,
    MODBUS_ERR_INVALID_ADDRESS  = -4,
    MODBUS_ERR_INVALID_COUNT    = -5,
    MODBUS_ERR_CRC              = -6,
    MODBUS_ERR_TIMEOUT          = -7,
    MODBUS_ERR_SEND             = -8,
    MODBUS_ERR_RECV             = -9,
    MODBUS_ERR_EXCEPTION        = -10,
    MODBUS_ERR_BUFSIZE          = -11,
    MODBUS_ERR_NOT_CONNECTED    = -12
} modbus_error_t;

/* 串口后端结构 */
typedef struct modbus_backend modbus_backend_t;

/* Modbus上下文结构 */
typedef struct {
    uint8_t slave;                      /* 从机地址 */
    uint32_t response_timeout_ms;       /* 响应超时(ms) */
    uint32_t byte_timeout_ms;           /* 字节间超时(ms) */
    uint32_t frame_delay_us;            /* 帧间延迟(us) */

    /* 后端接口 */
    const modbus_backend_t *backend;
    void *backend_data;

    /* 调试标志 */
    bool debug;

    /* 统计 */
    uint64_t msgs_sent;
    uint64_t msgs_received;
    uint64_t msgs_failed;
    uint64_t bytes_sent;
    uint64_t bytes_received;
} modbus_context_t;

/* 数据映射回调函数类型 */
typedef int (*modbus_read_bit_cb)(void *ctx, uint16_t addr, bool *value);
typedef int (*modbus_write_bit_cb)(void *ctx, uint16_t addr, bool value);
typedef int (*modbus_read_reg_cb)(void *ctx, uint16_t addr, uint16_t *value);
typedef int (*modbus_write_reg_cb)(void *ctx, uint16_t addr, uint16_t value);

/* 从机数据映射 */
typedef struct {
    /* 线圈 (0x01, 0x05, 0x0F) */
    uint16_t coil_start;
    uint16_t coil_count;
    modbus_read_bit_cb read_coil;
    modbus_write_bit_cb write_coil;

    /* 离散输入 (0x02) */
    uint16_t discrete_start;
    uint16_t discrete_count;
    modbus_read_bit_cb read_discrete;

    /* 保持寄存器 (0x03, 0x06, 0x10) */
    uint16_t holding_start;
    uint16_t holding_count;
    modbus_read_reg_cb read_holding;
    modbus_write_reg_cb write_holding;

    /* 输入寄存器 (0x04) */
    uint16_t input_start;
    uint16_t input_count;
    modbus_read_reg_cb read_input;

    /* 用户上下文 */
    void *user_ctx;
} modbus_slave_map_t;

/* 后端接口 */
struct modbus_backend {
    const char *name;
    size_t header_length;
    size_t checksum_length;
    size_t max_adu_length;

    int (*init)(void **backend_data, const char *device, int baudrate);
    void (*close)(void *backend_data);
    int (*send)(void *backend_data, const uint8_t *data, size_t len);
    int (*recv)(void *backend_data, uint8_t *buffer, size_t max_len,
                uint32_t timeout_ms, uint32_t byte_timeout_ms);
    int (*flush)(void *backend_data);
    void (*pre_send)(void *backend_data);
    void (*post_send)(void *backend_data);
    uint16_t (*checksum)(const uint8_t *data, size_t len);
    int (*check_integrity)(modbus_context_t *ctx, uint8_t *msg, size_t msg_len);
};

/* 函数声明 - 上下文管理 */
modbus_context_t* modbus_new_rtu(const char *device, int baudrate,
                                  char parity, int data_bits, int stop_bits);
void modbus_free(modbus_context_t *ctx);
int modbus_set_slave(modbus_context_t *ctx, uint8_t slave);
int modbus_connect(modbus_context_t *ctx);
void modbus_close(modbus_context_t *ctx);
int modbus_flush(modbus_context_t *ctx);
void modbus_set_debug(modbus_context_t *ctx, bool debug);

/* 函数声明 - 超时设置 */
void modbus_set_response_timeout(modbus_context_t *ctx, uint32_t timeout_ms);
void modbus_set_byte_timeout(modbus_context_t *ctx, uint32_t timeout_ms);
void modbus_get_response_timeout(modbus_context_t *ctx, uint32_t *timeout_ms);
void modbus_get_byte_timeout(modbus_context_t *ctx, uint32_t *timeout_ms);

/* 函数声明 - 主站功能 */
int modbus_read_bits(modbus_context_t *ctx, uint16_t addr, uint16_t nb,
                     uint8_t *dest);
int modbus_read_input_bits(modbus_context_t *ctx, uint16_t addr, uint16_t nb,
                           uint8_t *dest);
int modbus_read_registers(modbus_context_t *ctx, uint16_t addr, uint16_t nb,
                          uint16_t *dest);
int modbus_read_input_registers(modbus_context_t *ctx, uint16_t addr,
                                uint16_t nb, uint16_t *dest);
int modbus_write_bit(modbus_context_t *ctx, uint16_t addr, bool status);
int modbus_write_register(modbus_context_t *ctx, uint16_t addr,
                          uint16_t value);
int modbus_write_bits(modbus_context_t *ctx, uint16_t addr, uint16_t nb,
                      const uint8_t *data);
int modbus_write_registers(modbus_context_t *ctx, uint16_t addr, uint16_t nb,
                           const uint16_t *data);
int modbus_mask_write_register(modbus_context_t *ctx, uint16_t addr,
                               uint16_t and_mask, uint16_t or_mask);
int modbus_write_and_read_registers(modbus_context_t *ctx,
                                    uint16_t write_addr, uint16_t write_nb,
                                    const uint16_t *write_data,
                                    uint16_t read_addr, uint16_t read_nb,
                                    uint16_t *read_dest);

/* 函数声明 - 从站功能 */
int modbus_slave_poll(modbus_context_t *ctx, const modbus_slave_map_t *map,
                      uint8_t *req_buffer, size_t req_buffer_len);
int modbus_reply_exception(modbus_context_t *ctx, const uint8_t *req,
                           modbus_exception_code_t exception);

/* 函数声明 - 工具函数 */
uint16_t modbus_crc16(const uint8_t *data, size_t len);
const char* modbus_strerror(int errnum);
const char* modbus_exception_str(modbus_exception_code_t code);
void modbus_get_stats(modbus_context_t *ctx, uint64_t *sent, uint64_t *received,
                      uint64_t *failed, uint64_t *bytes_sent,
                      uint64_t *bytes_received);

#ifdef __cplusplus
}
#endif

#endif /* MODBUS_RTU_H */


