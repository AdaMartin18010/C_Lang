/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\IoT_Protocols\01_IoT_Protocols_Implementation.md
 * Line: 2653
 * Language: c
 * Block ID: bd8daa09
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file modbus.c
 * @brief Modbus协议实现
 */

#include "modbus.h"
#include <string.h>
#include <stdio.h>

/* CRC16查找表 (Modbus多项式 0xA001) */
static const uint16_t crc16_table[256] = {
    0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,
    0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,
    0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,
    0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,
    0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,
    0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,
    0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
    0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,
    0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,
    0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,
    0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,
    0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,
    0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,
    0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
    0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,
    0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,
    0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,
    0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
    0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,
    0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,
    0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
    0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
    0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,
    0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,
    0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,
    0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,
    0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,
    0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
    0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,
    0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,
    0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,
    0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040
};

/* 计算CRC16 */
uint16_t modbus_crc16(const uint8_t *data, size_t len) {
    uint16_t crc = 0xFFFF;
    for (size_t i = 0; i < len; i++) {
        crc = (crc >> 8) ^ crc16_table[(crc ^ data[i]) & 0xFF];
    }
    return crc;
}

/* 检查CRC16 */
bool modbus_check_crc16(const uint8_t *data, size_t len) {
    if (len < 2) return false;
    uint16_t calc_crc = modbus_crc16(data, len - 2);
    uint16_t recv_crc = (uint16_t)((data[len - 1] << 8) | data[len - 2]);
    return calc_crc == recv_crc;
}

/* 初始化RTU模式 */
int modbus_init_rtu(modbus_context_t *ctx, void *transport_ctx,
                     modbus_send_t send_func, modbus_recv_t recv_func,
                     modbus_flush_t flush_func, uint8_t slave_id) {
    if (ctx == NULL || send_func == NULL || recv_func == NULL) {
        return MODBUS_ERR_INVALID_PARAM;
    }

    memset(ctx, 0, sizeof(modbus_context_t));
    ctx->mode = MODBUS_RTU;
    ctx->slave_id = slave_id;
    ctx->transport_ctx = transport_ctx;
    ctx->send = send_func;
    ctx->recv = recv_func;
    ctx->flush = flush_func;
    ctx->response_timeout_ms = 500;
    ctx->byte_timeout_ms = 50;

    return MODBUS_ERR_OK;
}

/* 初始化TCP模式 */
int modbus_init_tcp(modbus_context_t *ctx, void *transport_ctx,
                     modbus_send_t send_func, modbus_recv_t recv_func) {
    if (ctx == NULL || send_func == NULL || recv_func == NULL) {
        return MODBUS_ERR_INVALID_PARAM;
    }

    memset(ctx, 0, sizeof(modbus_context_t));
    ctx->mode = MODBUS_TCP;
    ctx->transport_ctx = transport_ctx;
    ctx->send = send_func;
    ctx->recv = recv_func;
    ctx->response_timeout_ms = 500;
    ctx->byte_timeout_ms = 0;
    ctx->transaction_id = 1;

    return MODBUS_ERR_OK;
}

/* 构建RTU ADU */
static int build_rtu_adu(modbus_context_t *ctx, uint8_t slave_id,
                          uint8_t function, const uint8_t *data, size_t data_len) {
    int pos = 0;

    ctx->tx_buffer[pos++] = slave_id;
    ctx->tx_buffer[pos++] = function;

    if (data_len > 0) {
        memcpy(ctx->tx_buffer + pos, data, data_len);
        pos += (int)data_len;
    }

    /* 添加CRC */
    uint16_t crc = modbus_crc16(ctx->tx_buffer, pos);
    ctx->tx_buffer[pos++] = (uint8_t)(crc & 0xFF);
    ctx->tx_buffer[pos++] = (uint8_t)(crc >> 8);

    return pos;
}

/* 构建TCP ADU */
static int build_tcp_adu(modbus_context_t *ctx, uint8_t slave_id,
                          uint8_t function, const uint8_t *data, size_t data_len) {
    int pos = 0;

    /* MBAP头 */
    ctx->tx_buffer[pos++] = (uint8_t)(ctx->transaction_id >> 8);
    ctx->tx_buffer[pos++] = (uint8_t)(ctx->transaction_id & 0xFF);
    ctx->tx_buffer[pos++] = 0x00; /* 协议标识符高字节 */
    ctx->tx_buffer[pos++] = 0x00; /* 协议标识符低字节 */
    ctx->tx_buffer[pos++] = 0x00; /* 长度高字节 */
    ctx->tx_buffer[pos++] = (uint8_t)(data_len + 2); /* 长度低字节 */
    ctx->tx_buffer[pos++] = slave_id;
    ctx->tx_buffer[pos++] = function;

    if (data_len > 0) {
        memcpy(ctx->tx_buffer + pos, data, data_len);
        pos += (int)data_len;
    }

    ctx->transaction_id++;
    if (ctx->transaction_id == 0) ctx->transaction_id = 1;

    return pos;
}

/* 发送请求并接收响应 */
static int send_request(modbus_context_t *ctx, uint8_t slave_id,
                         uint8_t function, const uint8_t *data, size_t data_len,
                         uint8_t **response, size_t *resp_len) {
    int adu_len;

    if (ctx->mode == MODBUS_RTU) {
        adu_len = build_rtu_adu(ctx, slave_id, function, data, data_len);
    } else {
        adu_len = build_tcp_adu(ctx, slave_id, function, data, data_len);
    }

    /* 清空接收缓冲区 */
    if (ctx->flush) {
        ctx->flush(ctx->transport_ctx);
    }

    /* 发送 */
    if (ctx->send(ctx->transport_ctx, ctx->tx_buffer, adu_len) != adu_len) {
        return MODBUS_ERR_SEND;
    }
    ctx->tx_count++;

    /* 接收响应 */
    int rx_len = ctx->recv(ctx->transport_ctx, ctx->rx_buffer, MODBUS_MAX_ADU_SIZE,
                            ctx->response_timeout_ms);

    if (rx_len < 0) {
        ctx->error_count++;
        return MODBUS_ERR_RECV;
    }

    if (rx_len == 0) {
        ctx->timeout_count++;
        return MODBUS_ERR_TIMEOUT;
    }

    ctx->rx_count++;

    /* 验证响应 */
    if (ctx->mode == MODBUS_RTU) {
        if (rx_len < 5 || !modbus_check_crc16(ctx->rx_buffer, rx_len)) {
            ctx->error_count++;
            return MODBUS_ERR_CRC;
        }
        *response = ctx->rx_buffer + 2; /* 跳过slave_id和function */
        *resp_len = rx_len - 4; /* 减去slave_id, function, CRC */
    } else {
        if (rx_len < MODBUS_TCP_MBAP_SIZE + 2) {
            ctx->error_count++;
            return MODBUS_ERR_RECV;
        }
        *response = ctx->rx_buffer + MODBUS_TCP_MBAP_SIZE + 1; /* 跳过MBAP和slave_id */
        *resp_len = rx_len - MODBUS_TCP_MBAP_SIZE - 1;
    }

    /* 检查功能码 */
    uint8_t resp_func = (ctx->mode == MODBUS_RTU) ? ctx->rx_buffer[1]
                                                   : ctx->rx_buffer[MODBUS_TCP_MBAP_SIZE];

    if (resp_func != function) {
        if (resp_func == (function | 0x80)) {
            /* 异常响应 */
            if (*resp_len >= 2) {
                return (*response)[1] | 0x100; /* 返回异常码 + 标志 */
            }
        }
        return MODBUS_ERR_INVALID_FUNCTION;
    }

    *resp_len -= 1; /* 减去function字节 */

    return MODBUS_ERR_OK;
}

/* 读取线圈 */
int modbus_read_bits(modbus_context_t *ctx, uint8_t slave_id, uint16_t addr,
                      uint16_t count, uint8_t *values) {
    if (ctx == NULL || values == NULL || count == 0 || count > MODBUS_MAX_READ_BITS) {
        return MODBUS_ERR_INVALID_PARAM;
    }

    uint8_t req[4];
    modbus_set_u16(req, addr);
    modbus_set_u16(req + 2, count);

    uint8_t *resp;
    size_t resp_len;
    int err = send_request(ctx, slave_id, MODBUS_FC_READ_COILS, req, 4, &resp, &resp_len);

    if (err < 0) return err;
    if (err >= 0x100) return MODBUS_ERR_EXCEPTION;

    if (resp_len < 1 || resp[0] != (count + 7) / 8) {
        return MODBUS_ERR_RECV;
    }

    /* 解压位数据 */
    for (uint16_t i = 0; i < count; i++) {
        values[i] = (resp[1 + i / 8] >> (i % 8)) & 0x01;
    }

    return (int)count;
}

/* 读取离散输入 */
int modbus_read_input_bits(modbus_context_t *ctx, uint8_t slave_id, uint16_t addr,
                            uint16_t count, uint8_t *values) {
    if (ctx == NULL || values == NULL || count == 0 || count > MODBUS_MAX_READ_BITS) {
        return MODBUS_ERR_INVALID_PARAM;
    }

    uint8_t req[4];
    modbus_set_u16(req, addr);
    modbus_set_u16(req + 2, count);

    uint8_t *resp;
    size_t resp_len;
    int err = send_request(ctx, slave_id, MODBUS_FC_READ_DISCRETE_INPUTS, req, 4, &resp, &resp_len);

    if (err < 0) return err;
    if (err >= 0x100) return MODBUS_ERR_EXCEPTION;

    if (resp_len < 1 || resp[0] != (count + 7) / 8) {
        return MODBUS_ERR_RECV;
    }

    for (uint16_t i = 0; i < count; i++) {
        values[i] = (resp[1 + i / 8] >> (i % 8)) & 0x01;
    }

    return (int)count;
}

/* 读取保持寄存器 */
int modbus_read_registers(modbus_context_t *ctx, uint8_t slave_id, uint16_t addr,
                           uint16_t count, uint16_t *values) {
    if (ctx == NULL || values == NULL || count == 0 || count > MODBUS_MAX_READ_REGISTERS) {
        return MODBUS_ERR_INVALID_PARAM;
    }

    uint8_t req[4];
    modbus_set_u16(req, addr);
    modbus_set_u16(req + 2, count);

    uint8_t *resp;
    size_t resp_len;
    int err = send_request(ctx, slave_id, MODBUS_FC_READ_HOLDING_REGISTERS, req, 4, &resp, &resp_len);

    if (err < 0) return err;
    if (err >= 0x100) return MODBUS_ERR_EXCEPTION;

    if (resp_len < 1 || resp[0] != count * 2) {
        return MODBUS_ERR_RECV;
    }

    for (uint16_t i = 0; i < count; i++) {
        values[i] = modbus_get_u16(resp + 1 + i * 2);
    }

    return (int)count;
}

/* 读取输入寄存器 */
int modbus_read_input_registers(modbus_context_t *ctx, uint8_t slave_id, uint16_t addr,
                                 uint16_t count, uint16_t *values) {
    if (ctx == NULL || values == NULL || count == 0 || count > MODBUS_MAX_READ_REGISTERS) {
        return MODBUS_ERR_INVALID_PARAM;
    }

    uint8_t req[4];
    modbus_set_u16(req, addr);
    modbus_set_u16(req + 2, count);

    uint8_t *resp;
    size_t resp_len;
    int err = send_request(ctx, slave_id, MODBUS_FC_READ_INPUT_REGISTERS, req, 4, &resp, &resp_len);

    if (err < 0) return err;
    if (err >= 0x100) return MODBUS_ERR_EXCEPTION;

    if (resp_len < 1 || resp[0] != count * 2) {
        return MODBUS_ERR_RECV;
    }

    for (uint16_t i = 0; i < count; i++) {
        values[i] = modbus_get_u16(resp + 1 + i * 2);
    }

    return (int)count;
}

/* 写单个线圈 */
int modbus_write_bit(modbus_context_t *ctx, uint8_t slave_id, uint16_t addr, bool value) {
    if (ctx == NULL) {
        return MODBUS_ERR_INVALID_PARAM;
    }

    uint8_t req[4];
    modbus_set_u16(req, addr);
    modbus_set_u16(req + 2, value ? 0xFF00 : 0x0000);

    uint8_t *resp;
    size_t resp_len;
    int err = send_request(ctx, slave_id, MODBUS_FC_WRITE_SINGLE_COIL, req, 4, &resp, &resp_len);

    if (err < 0) return err;
    if (err >= 0x100) return MODBUS_ERR_EXCEPTION;

    return MODBUS_ERR_OK;
}

/* 写单个寄存器 */
int modbus_write_register(modbus_context_t *ctx, uint8_t slave_id, uint16_t addr, uint16_t value) {
    if (ctx == NULL) {
        return MODBUS_ERR_INVALID_PARAM;
    }

    uint8_t req[4];
    modbus_set_u16(req, addr);
    modbus_set_u16(req + 2, value);

    uint8_t *resp;
    size_t resp_len;
    int err = send_request(ctx, slave_id, MODBUS_FC_WRITE_SINGLE_REGISTER, req, 4, &resp, &resp_len);

    if (err < 0) return err;
    if (err >= 0x100) return MODBUS_ERR_EXCEPTION;

    return MODBUS_ERR_OK;
}

/* 写多个线圈 */
int modbus_write_bits(modbus_context_t *ctx, uint8_t slave_id, uint16_t addr,
                       uint16_t count, const uint8_t *values) {
    if (ctx == NULL || values == NULL || count == 0 || count > MODBUS_MAX_WRITE_BITS) {
        return MODBUS_ERR_INVALID_PARAM;
    }

    uint8_t req[256];
    int byte_count = (count + 7) / 8;

    modbus_set_u16(req, addr);
    modbus_set_u16(req + 2, count);
    req[4] = (uint8_t)byte_count;

    memset(req + 5, 0, byte_count);
    for (uint16_t i = 0; i < count; i++) {
        if (values[i]) {
            req[5 + i / 8] |= (1 << (i % 8));
        }
    }

    uint8_t *resp;
    size_t resp_len;
    int err = send_request(ctx, slave_id, MODBUS_FC_WRITE_MULTIPLE_COILS, req, 5 + byte_count, &resp, &resp_len);

    if (err < 0) return err;
    if (err >= 0x100) return MODBUS_ERR_EXCEPTION;

    return MODBUS_ERR_OK;
}

/* 写多个寄存器 */
int modbus_write_registers(modbus_context_t *ctx, uint8_t slave_id, uint16_t addr,
                            uint16_t count, const uint16_t *values) {
    if (ctx == NULL || values == NULL || count == 0 || count > MODBUS_MAX_WRITE_REGISTERS) {
        return MODBUS_ERR_INVALID_PARAM;
    }

    uint8_t req[256];
    modbus_set_u16(req, addr);
    modbus_set_u16(req + 2, count);
    req[4] = (uint8_t)(count * 2);

    for (uint16_t i = 0; i < count; i++) {
        modbus_set_u16(req + 5 + i * 2, values[i]);
    }

    uint8_t *resp;
    size_t resp_len;
    int err = send_request(ctx, slave_id, MODBUS_FC_WRITE_MULTIPLE_REGISTERS, req, 5 + count * 2, &resp, &resp_len);

    if (err < 0) return err;
    if (err >= 0x100) return MODBUS_ERR_EXCEPTION;

    return MODBUS_ERR_OK;
}

/* 掩码写寄存器 */
int modbus_mask_write_register(modbus_context_t *ctx, uint8_t slave_id, uint16_t addr,
                                uint16_t and_mask, uint16_t or_mask) {
    if (ctx == NULL) {
        return MODBUS_ERR_INVALID_PARAM;
    }

    uint8_t req[6];
    modbus_set_u16(req, addr);
    modbus_set_u16(req + 2, and_mask);
    modbus_set_u16(req + 4, or_mask);

    uint8_t *resp;
    size_t resp_len;
    int err = send_request(ctx, slave_id, MODBUS_FC_MASK_WRITE_REGISTER, req, 6, &resp, &resp_len);

    if (err < 0) return err;
    if (err >= 0x100) return MODBUS_ERR_EXCEPTION;

    return MODBUS_ERR_OK;
}

/* 错误码转字符串 */
const char* modbus_strerror(modbus_error_t err) {
    switch (err) {
        case MODBUS_ERR_OK: return "No error";
        case MODBUS_ERR_INVALID_PARAM: return "Invalid parameter";
        case MODBUS_ERR_TIMEOUT: return "Timeout";
        case MODBUS_ERR_CRC: return "CRC error";
        case MODBUS_ERR_EXCEPTION: return "Modbus exception";
        case MODBUS_ERR_SEND: return "Send failed";
        case MODBUS_ERR_RECV: return "Receive failed";
        case MODBUS_ERR_INVALID_SLAVE: return "Invalid slave ID";
        case MODBUS_ERR_INVALID_FUNCTION: return "Invalid function code";
        default: return "Unknown error";
    }
}

/* 异常码转字符串 */
const char* modbus_exception_str(modbus_exception_code_t exc) {
    switch (exc) {
        case MODBUS_EX_NONE: return "No exception";
        case MODBUS_EX_ILLEGAL_FUNCTION: return "Illegal function";
        case MODBUS_EX_ILLEGAL_DATA_ADDRESS: return "Illegal data address";
        case MODBUS_EX_ILLEGAL_DATA_VALUE: return "Illegal data value";
        case MODBUS_EX_SLAVE_DEVICE_FAILURE: return "Slave device failure";
        case MODBUS_EX_ACKNOWLEDGE: return "Acknowledge";
        case MODBUS_EX_SLAVE_DEVICE_BUSY: return "Slave device busy";
        case MODBUS_EX_NEGATIVE_ACKNOWLEDGE: return "Negative acknowledge";
        case MODBUS_EX_MEMORY_PARITY_ERROR: return "Memory parity error";
        case MODBUS_EX_GATEWAY_PATH_UNAVAILABLE: return "Gateway path unavailable";
        case MODBUS_EX_GATEWAY_TARGET_FAILED: return "Gateway target device failed";
        default: return "Unknown exception";
    }
}
