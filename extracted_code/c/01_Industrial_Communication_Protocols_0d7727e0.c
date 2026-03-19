/*
 * Auto-generated from: 04_Industrial_Scenarios\15_Industrial_Communication\01_Industrial_Communication_Protocols.md
 * Line: 180
 * Language: c
 * Block ID: 0d7727e0
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file serial_port.h
 * @brief RS-232/RS-485串口通信头文件
 * @version 1.0.0
 * @date 2024
 */

#ifndef SERIAL_PORT_H
#define SERIAL_PORT_H

#include <stdint.h>
#include <stdbool.h>
#include <termios.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 串口配置结构体 */
typedef struct {
    char device[64];        /* 设备路径,如 "/dev/ttyUSB0" */
    uint32_t baudrate;      /* 波特率 */
    uint8_t databits;       /* 数据位: 5,6,7,8 */
    uint8_t stopbits;       /* 停止位: 1,2 */
    char parity;            /* 校验: 'N'=无, 'O'=奇, 'E'=偶 */
    uint32_t timeout_ms;    /* 读超时时间(ms) */
    bool rs485_mode;        /* RS-485模式 */
    bool rts_on_send;       /* 发送时RTS有效 */
} serial_config_t;

/* 串口句柄 */
typedef struct {
    int fd;                     /* 文件描述符 */
    serial_config_t config;     /* 配置信息 */
    struct termios old_tio;     /* 原始配置(用于恢复) */
    uint64_t tx_bytes;          /* 发送字节计数 */
    uint64_t rx_bytes;          /* 接收字节计数 */
    uint64_t tx_errors;         /* 发送错误计数 */
    uint64_t rx_errors;         /* 接收错误计数 */
} serial_handle_t;

/* 波特率定义 */
#define SERIAL_BAUD_9600    9600
#define SERIAL_BAUD_19200   19200
#define SERIAL_BAUD_38400   38400
#define SERIAL_BAUD_57600   57600
#define SERIAL_BAUD_115200  115200
#define SERIAL_BAUD_230400  230400
#define SERIAL_BAUD_460800  460800
#define SERIAL_BAUD_921600  921600

/* 函数声明 */
int serial_init(serial_handle_t *handle, const serial_config_t *config);
void serial_close(serial_handle_t *handle);
int serial_send(serial_handle_t *handle, const uint8_t *data, size_t len);
int serial_receive(serial_handle_t *handle, uint8_t *buffer, size_t max_len, uint32_t timeout_ms);
int serial_flush(serial_handle_t *handle);
int serial_set_dtr(serial_handle_t *handle, bool state);
int serial_set_rts(serial_handle_t *handle, bool state);
int serial_get_modem_status(serial_handle_t *handle);
void serial_get_stats(serial_handle_t *handle, uint64_t *tx, uint64_t *rx, uint64_t *tx_err, uint64_t *rx_err);
const char* serial_strerror(int error);

#ifdef __cplusplus
}
#endif

#endif /* SERIAL_PORT_H */
