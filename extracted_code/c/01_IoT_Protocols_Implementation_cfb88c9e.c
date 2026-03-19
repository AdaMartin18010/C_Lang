/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\IoT_Protocols\01_IoT_Protocols_Implementation.md
 * Line: 4924
 * Language: c
 * Block ID: cfb88c9e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file websocket.h
 * @brief 轻量级WebSocket客户端
 */

#ifndef WEBSOCKET_H
#define WEBSOCKET_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* WebSocket操作码 */
typedef enum {
    WS_OP_CONTINUATION = 0x0,
    WS_OP_TEXT = 0x1,
    WS_OP_BINARY = 0x2,
    WS_OP_CLOSE = 0x8,
    WS_OP_PING = 0x9,
    WS_OP_PONG = 0xA
} ws_opcode_t;

/* 状态 */
typedef enum {
    WS_STATE_CLOSED = 0,
    WS_STATE_CONNECTING = 1,
    WS_STATE_OPEN = 2,
    WS_STATE_CLOSING = 3
} ws_state_t;

/* 关闭码 */
typedef enum {
    WS_CLOSE_NORMAL = 1000,
    WS_CLOSE_GOING_AWAY = 1001,
    WS_CLOSE_PROTOCOL_ERROR = 1002,
    WS_CLOSE_UNSUPPORTED = 1003,
    WS_CLOSE_RESERVED = 1004,
    WS_CLOSE_NO_STATUS = 1005,
    WS_CLOSE_ABNORMAL = 1006,
    WS_CLOSE_INVALID_DATA = 1007,
    WS_CLOSE_POLICY_VIOLATION = 1008,
    WS_CLOSE_TOO_LARGE = 1009,
    WS_CLOSE_EXTENSION_REQUIRED = 1010,
    WS_CLOSE_SERVER_ERROR = 1011
} ws_close_code_t;

/* 最大配置 */
#define WS_MAX_URL_LEN          256
#define WS_MAX_HOST_LEN         128
#define WS_MAX_PATH_LEN         128
#define WS_MAX_FRAME_SIZE       4096
#define WS_MAX_PAYLOAD_SIZE     2048
#define WS_MAX_HANDSHAKE_LEN    1024

/* 错误码 */
typedef enum {
    WS_ERR_OK = 0,
    WS_ERR_INVALID_PARAM = -1,
    WS_ERR_NO_MEMORY = -2,
    WS_ERR_CONNECT_FAIL = -3,
    WS_ERR_HANDSHAKE_FAIL = -4,
    WS_ERR_SEND_FAIL = -5,
    WS_ERR_RECV_FAIL = -6,
    WS_ERR_TIMEOUT = -7,
    WS_ERR_PROTOCOL = -8,
    WS_ERR_NOT_CONNECTED = -9
} ws_error_t;

/* 传输层回调 */
typedef int (*ws_tcp_connect_t)(void *ctx, const char *host, uint16_t port);
typedef int (*ws_tcp_send_t)(void *ctx, const uint8_t *data, size_t len);
typedef int (*ws_tcp_recv_t)(void *ctx, uint8_t *data, size_t max_len, uint32_t timeout_ms);
typedef void (*ws_tcp_close_t)(void *ctx);

/* 事件回调 */
typedef void (*ws_on_open_t)(void *user_data);
typedef void (*ws_on_message_t)(ws_opcode_t type, const uint8_t *data,
                                 size_t len, void *user_data);
typedef void (*ws_on_close_t)(uint16_t code, const char *reason, void *user_data);
typedef void (*ws_on_error_t)(ws_error_t err, void *user_data);

/* WebSocket帧 */
typedef struct {
    bool fin;               /* 是否为最后一帧 */
    bool rsv1, rsv2, rsv3;  /* 保留位 */
    ws_opcode_t opcode;     /* 操作码 */
    bool masked;            /* 是否掩码 */
    uint64_t payload_len;   /* 载荷长度 */
    uint8_t masking_key[4]; /* 掩码密钥 */
    uint8_t payload[WS_MAX_PAYLOAD_SIZE]; /* 载荷 */
} ws_frame_t;

/* WebSocket客户端 */
typedef struct {
    /* 传输层 */
    void *tcp_ctx;
    ws_tcp_connect_t tcp_connect;
    ws_tcp_send_t tcp_send;
    ws_tcp_recv_t tcp_recv;
    ws_tcp_close_t tcp_close;

    /* 状态 */
    ws_state_t state;
    char host[WS_MAX_HOST_LEN];
    char path[WS_MAX_PATH_LEN];
    uint16_t port;
    bool use_ssl;

    /* 回调 */
    ws_on_open_t on_open;
    ws_on_message_t on_message;
    ws_on_close_t on_close;
    ws_on_error_t on_error;
    void *user_data;

    /* 缓冲区 */
    uint8_t tx_buffer[WS_MAX_FRAME_SIZE];
    uint8_t rx_buffer[WS_MAX_FRAME_SIZE];
    size_t rx_buffered;

    /* 分帧重组 */
    uint8_t fragmented[WS_MAX_PAYLOAD_SIZE];
    size_t fragmented_len;
    ws_opcode_t fragmented_type;
    bool is_fragmented;

    /* 保活 */
    uint32_t keepalive_interval_ms;
    uint32_t last_activity_ms;
} ws_client_t;

/* API函数 */

/* 初始化 */
int ws_init(ws_client_t *client, void *tcp_ctx,
             ws_tcp_connect_t connect, ws_tcp_send_t send,
             ws_tcp_recv_t recv, ws_tcp_close_t close);

/* 连接 */
int ws_connect(ws_client_t *client, const char *url);
void ws_set_callbacks(ws_client_t *client,
                       ws_on_open_t on_open,
                       ws_on_message_t on_message,
                       ws_on_close_t on_close,
                       ws_on_error_t on_error,
                       void *user_data);

/* 发送 */
int ws_send_text(ws_client_t *client, const char *text);
int ws_send_binary(ws_client_t *client, const uint8_t *data, size_t len);
int ws_ping(ws_client_t *client);
int ws_pong(ws_client_t *client, const uint8_t *data, size_t len);

/* 关闭 */
int ws_close(ws_client_t *client, uint16_t code, const char *reason);

/* 处理循环 */
int ws_process(ws_client_t *client, uint32_t timeout_ms);
ws_state_t ws_get_state(ws_client_t *client);
bool ws_is_connected(ws_client_t *client);

/* 配置 */
void ws_set_keepalive(ws_client_t *client, uint32_t interval_ms);

/* 工具函数 */
ws_error_t ws_get_last_error(ws_client_t *client);
const char* ws_err_str(ws_error_t err);

#ifdef __cplusplus
}
#endif

#endif /* WEBSOCKET_H */
