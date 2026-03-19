/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\IoT_Protocols\01_IoT_Protocols_Implementation.md
 * Line: 5104
 * Language: c
 * Block ID: 6d837ef3
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file websocket.c
 * @brief WebSocket客户端实现
 */

#include "websocket.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/* Base64编码表 */
static const char base64_chars[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/* SHA1实现 (简化) */
static void sha1(const uint8_t *data, size_t len, uint8_t hash[20]) {
    /* 简化实现 - 实际应使用完整SHA1 */
    memset(hash, 0, 20);
    (void)data;
    (void)len;
}

/* Base64编码 */
static int base64_encode(const uint8_t *in, size_t in_len, char *out, size_t out_len) {
    size_t i, j;
    for (i = 0, j = 0; i < in_len; i += 3, j += 4) {
        if (j + 4 >= out_len) return -1;

        uint32_t val = (in[i] << 16);
        if (i + 1 < in_len) val |= (in[i + 1] << 8);
        if (i + 2 < in_len) val |= in[i + 2];

        out[j] = base64_chars[(val >> 18) & 0x3F];
        out[j + 1] = base64_chars[(val >> 12) & 0x3F];
        out[j + 2] = (i + 1 < in_len) ? base64_chars[(val >> 6) & 0x3F] : '=';
        out[j + 3] = (i + 2 < in_len) ? base64_chars[val & 0x3F] : '=';
    }
    out[j] = '\0';
    return 0;
}

/* 生成WebSocket密钥 */
static void generate_key(char *key, size_t key_len) {
    uint8_t random_bytes[16];
    for (int i = 0; i < 16; i++) {
        random_bytes[i] = (uint8_t)(rand() % 256);
    }
    base64_encode(random_bytes, 16, key, key_len);
}

/* 解析URL */
static int parse_url(const char *url, char *host, size_t host_len,
                      char *path, size_t path_len, uint16_t *port, bool *use_ssl) {
    const char *p = url;
    *use_ssl = false;
    *port = 80;

    if (strncmp(p, "wss://", 6) == 0) {
        *use_ssl = true;
        *port = 443;
        p += 6;
    } else if (strncmp(p, "ws://", 5) == 0) {
        p += 5;
    } else if (strncmp(p, "https://", 8) == 0) {
        *use_ssl = true;
        *port = 443;
        p += 8;
    } else if (strncmp(p, "http://", 7) == 0) {
        p += 7;
    }

    const char *slash = strchr(p, '/');
    const char *colon = strchr(p, ':');

    size_t host_end;
    if (colon != NULL && (slash == NULL || colon < slash)) {
        host_end = (size_t)(colon - p);
        if (host_end >= host_len) host_end = host_len - 1;
        strncpy(host, p, host_end);
        host[host_end] = '\0';

        *port = (uint16_t)atoi(colon + 1);

        p = (slash != NULL) ? slash : "/";
    } else {
        if (slash != NULL) {
            host_end = (size_t)(slash - p);
        } else {
            host_end = strlen(p);
        }
        if (host_end >= host_len) host_end = host_len - 1;
        strncpy(host, p, host_end);
        host[host_end] = '\0';

        p = (slash != NULL) ? slash : "/";
    }

    strncpy(path, p, path_len - 1);
    path[path_len - 1] = '\0';

    return 0;
}

/* 初始化 */
int ws_init(ws_client_t *client, void *tcp_ctx,
             ws_tcp_connect_t connect, ws_tcp_send_t send,
             ws_tcp_recv_t recv, ws_tcp_close_t close) {
    if (client == NULL || connect == NULL || send == NULL ||
        recv == NULL || close == NULL) {
        return WS_ERR_INVALID_PARAM;
    }

    memset(client, 0, sizeof(ws_client_t));
    client->tcp_ctx = tcp_ctx;
    client->tcp_connect = connect;
    client->tcp_send = send;
    client->tcp_recv = recv;
    client->tcp_close = close;
    client->state = WS_STATE_CLOSED;
    client->keepalive_interval_ms = 30000;

    return WS_ERR_OK;
}

/* 设置回调 */
void ws_set_callbacks(ws_client_t *client,
                       ws_on_open_t on_open,
                       ws_on_message_t on_message,
                       ws_on_close_t on_close,
                       ws_on_error_t on_error,
                       void *user_data) {
    if (client == NULL) return;

    client->on_open = on_open;
    client->on_message = on_message;
    client->on_close = on_close;
    client->on_error = on_error;
    client->user_data = user_data;
}

/* 构建握手请求 */
static int build_handshake(ws_client_t *client, const char *key,
                            uint8_t *buffer, size_t max_len) {
    int n = snprintf((char *)buffer, max_len,
        "GET %s HTTP/1.1\r\n"
        "Host: %s:%d\r\n"
        "Upgrade: websocket\r\n"
        "Connection: Upgrade\r\n"
        "Sec-WebSocket-Key: %s\r\n"
        "Sec-WebSocket-Version: 13\r\n"
        "\r\n",
        client->path, client->host, client->port, key);

    return n;
}

/* 连接 */
int ws_connect(ws_client_t *client, const char *url) {
    if (client == NULL || url == NULL) {
        return WS_ERR_INVALID_PARAM;
    }

    client->state = WS_STATE_CONNECTING;

    /* 解析URL */
    if (parse_url(url, client->host, sizeof(client->host),
                   client->path, sizeof(client->path),
                   &client->port, &client->use_ssl) != 0) {
        client->state = WS_STATE_CLOSED;
        return WS_ERR_INVALID_PARAM;
    }

    /* TCP连接 */
    if (client->tcp_connect(client->tcp_ctx, client->host, client->port) != 0) {
        client->state = WS_STATE_CLOSED;
        return WS_ERR_CONNECT_FAIL;
    }

    /* WebSocket握手 */
    char key[32];
    generate_key(key, sizeof(key));

    int req_len = build_handshake(client, key, client->tx_buffer, sizeof(client->tx_buffer));
    if (client->tcp_send(client->tcp_ctx, client->tx_buffer, req_len) != req_len) {
        client->tcp_close(client->tcp_ctx);
        client->state = WS_STATE_CLOSED;
        return WS_ERR_HANDSHAKE_FAIL;
    }

    /* 接收响应 */
    int resp_len = client->tcp_recv(client->tcp_ctx, client->rx_buffer,
                                      sizeof(client->rx_buffer) - 1, 10000);
    if (resp_len <= 0) {
        client->tcp_close(client->tcp_ctx);
        client->state = WS_STATE_CLOSED;
        return WS_ERR_HANDSHAKE_FAIL;
    }
    client->rx_buffer[resp_len] = '\0';

    /* 验证响应 (简化) */
    if (strstr((char *)client->rx_buffer, "101") == NULL ||
        strstr((char *)client->rx_buffer, "websocket") == NULL) {
        client->tcp_close(client->tcp_ctx);
        client->state = WS_STATE_CLOSED;
        return WS_ERR_HANDSHAKE_FAIL;
    }

    client->state = WS_STATE_OPEN;
    client->rx_buffered = 0;
    client->fragmented_len = 0;
    client->is_fragmented = false;

    /* 获取当前时间 */
    client->last_activity_ms = 0; /* 应使用系统时间 */

    if (client->on_open != NULL) {
        client->on_open(client->user_data);
    }

    return WS_ERR_OK;
}

/* 构建帧 */
static int build_frame(const ws_frame_t *frame, uint8_t *buffer, size_t max_len) {
    size_t pos = 0;

    /* 首字节 */
    buffer[pos] = (frame->opcode & 0x0F);
    if (frame->fin) buffer[pos] |= 0x80;
    if (frame->rsv1) buffer[pos] |= 0x40;
    if (frame->rsv2) buffer[pos] |= 0x20;
    if (frame->rsv3) buffer[pos] |= 0x10;
    pos++;

    /* 长度和掩码 */
    uint8_t len_byte = frame->masked ? 0x80 : 0;

    if (frame->payload_len < 126) {
        len_byte |= (uint8_t)frame->payload_len;
        buffer[pos++] = len_byte;
    } else if (frame->payload_len < 65536) {
        len_byte |= 126;
        buffer[pos++] = len_byte;
        buffer[pos++] = (uint8_t)(frame->payload_len >> 8);
        buffer[pos++] = (uint8_t)(frame->payload_len & 0xFF);
    } else {
        len_byte |= 127;
        buffer[pos++] = len_byte;
        for (int i = 7; i >= 0; i--) {
            buffer[pos++] = (uint8_t)(frame->payload_len >> (i * 8));
        }
    }

    /* 掩码密钥 */
    if (frame->masked) {
        memcpy(buffer + pos, frame->masking_key, 4);
        pos += 4;
    }

    /* 载荷 (需要掩码) */
    if (pos + frame->payload_len > max_len) return WS_ERR_NO_MEMORY;

    if (frame->masked) {
        for (size_t i = 0; i < frame->payload_len; i++) {
            buffer[pos + i] = frame->payload[i] ^ frame->masking_key[i % 4];
        }
    } else {
        memcpy(buffer + pos, frame->payload, frame->payload_len);
    }
    pos += frame->payload_len;

    return (int)pos;
}

/* 发送帧 */
static int send_frame(ws_client_t *client, ws_opcode_t opcode,
                       const uint8_t *data, size_t len, bool masked) {
    if (client->state != WS_STATE_OPEN) {
        return WS_ERR_NOT_CONNECTED;
    }

    ws_frame_t frame;
    memset(&frame, 0, sizeof(frame));
    frame.fin = true;
    frame.opcode = opcode;
    frame.masked = masked;
    frame.payload_len = len;

    if (masked) {
        /* 生成随机掩码 */
        for (int i = 0; i < 4; i++) {
            frame.masking_key[i] = (uint8_t)(rand() % 256);
        }
    }

    if (len > 0 && data != NULL) {
        memcpy(frame.payload, data, len < WS_MAX_PAYLOAD_SIZE ? len : WS_MAX_PAYLOAD_SIZE);
    }

    int frame_len = build_frame(&frame, client->tx_buffer, sizeof(client->tx_buffer));
    if (frame_len < 0) return frame_len;

    if (client->tcp_send(client->tcp_ctx, client->tx_buffer, frame_len) != frame_len) {
        return WS_ERR_SEND_FAIL;
    }

    return WS_ERR_OK;
}

/* 发送文本 */
int ws_send_text(ws_client_t *client, const char *text) {
    if (text == NULL) return WS_ERR_INVALID_PARAM;
    return send_frame(client, WS_OP_TEXT, (const uint8_t *)text, strlen(text), true);
}

/* 发送二进制 */
int ws_send_binary(ws_client_t *client, const uint8_t *data, size_t len) {
    return send_frame(client, WS_OP_BINARY, data, len, true);
}

/* 发送Ping */
int ws_ping(ws_client_t *client) {
    return send_frame(client, WS_OP_PING, NULL, 0, true);
}

/* 发送Pong */
int ws_pong(ws_client_t *client, const uint8_t *data, size_t len) {
    return send_frame(client, WS_OP_PONG, data, len, true);
}

/* 关闭连接 */
int ws_close(ws_client_t *client, uint16_t code, const char *reason) {
    if (client->state == WS_STATE_CLOSED) {
        return WS_ERR_OK;
    }

    client->state = WS_STATE_CLOSING;

    /* 发送关闭帧 */
    uint8_t payload[125];
    payload[0] = (uint8_t)(code >> 8);
    payload[1] = (uint8_t)(code & 0xFF);

    size_t reason_len = 0;
    if (reason != NULL) {
        reason_len = strlen(reason);
        if (reason_len > 123) reason_len = 123;
        memcpy(payload + 2, reason, reason_len);
    }

    send_frame(client, WS_OP_CLOSE, payload, 2 + reason_len, true);

    /* 关闭TCP */
    client->tcp_close(client->tcp_ctx);
    client->state = WS_STATE_CLOSED;

    if (client->on_close != NULL) {
        client->on_close(code, reason, client->user_data);
    }

    return WS_ERR_OK;
}

/* 解析帧 */
static int parse_frame(ws_client_t *client, ws_frame_t *frame) {
    if (client->rx_buffered < 2) return 0; /* 需要更多数据 */

    size_t pos = 0;

    /* 首字节 */
    frame->fin = (client->rx_buffer[pos] & 0x80) != 0;
    frame->rsv1 = (client->rx_buffer[pos] & 0x40) != 0;
    frame->rsv2 = (client->rx_buffer[pos] & 0x20) != 0;
    frame->rsv3 = (client->rx_buffer[pos] & 0x10) != 0;
    frame->opcode = (ws_opcode_t)(client->rx_buffer[pos] & 0x0F);
    pos++;

    /* 长度和掩码 */
    frame->masked = (client->rx_buffer[pos] & 0x80) != 0;
    uint64_t payload_len = client->rx_buffer[pos] & 0x7F;
    pos++;

    if (payload_len == 126) {
        if (client->rx_buffered < pos + 2) return 0;
        payload_len = ((uint64_t)client->rx_buffer[pos] << 8) | client->rx_buffer[pos + 1];
        pos += 2;
    } else if (payload_len == 127) {
        if (client->rx_buffered < pos + 8) return 0;
        payload_len = 0;
        for (int i = 0; i < 8; i++) {
            payload_len = (payload_len << 8) | client->rx_buffer[pos + i];
        }
        pos += 8;
    }

    frame->payload_len = payload_len;

    /* 掩码密钥 */
    if (frame->masked) {
        if (client->rx_buffered < pos + 4) return 0;
        memcpy(frame->masking_key, client->rx_buffer + pos, 4);
        pos += 4;
    }

    /* 载荷 */
    if (client->rx_buffered < pos + payload_len) return 0;

    if (payload_len > WS_MAX_PAYLOAD_SIZE) {
        payload_len = WS_MAX_PAYLOAD_SIZE;
    }

    memcpy(frame->payload, client->rx_buffer + pos, (size_t)payload_len);

    /* 解掩码 */
    if (frame->masked) {
        for (size_t i = 0; i < payload_len; i++) {
            frame->payload[i] ^= frame->masking_key[i % 4];
        }
    }

    /* 移除已处理的数据 */
    size_t consumed = pos + payload_len;
    memmove(client->rx_buffer, client->rx_buffer + consumed,
            client->rx_buffered - consumed);
    client->rx_buffered -= consumed;

    return 1;
}

/* 处理循环 */
int ws_process(ws_client_t *client, uint32_t timeout_ms) {
    if (client == NULL || client->state != WS_STATE_OPEN) {
        return WS_ERR_NOT_CONNECTED;
    }

    /* 接收数据 */
    int rx_len = client->tcp_recv(client->tcp_ctx,
                                   client->rx_buffer + client->rx_buffered,
                                   sizeof(client->rx_buffer) - client->rx_buffered - 1,
                                   timeout_ms);

    if (rx_len > 0) {
        client->rx_buffered += rx_len;
        client->last_activity_ms = 0; /* 更新活动时间 */
    }

    /* 解析帧 */
    ws_frame_t frame;
    while (parse_frame(client, &frame) == 1) {
        switch (frame.opcode) {
            case WS_OP_TEXT:
            case WS_OP_BINARY:
                if (client->on_message != NULL) {
                    client->on_message(frame.opcode, frame.payload,
                                        (size_t)frame.payload_len, client->user_data);
                }
                break;

            case WS_OP_PING:
                /* 自动回复Pong */
                ws_pong(client, frame.payload, (size_t)frame.payload_len);
                break;

            case WS_OP_PONG:
                /* 收到Pong响应 */
                break;

            case WS_OP_CLOSE:
                /* 收到关闭帧 */
                {
                    uint16_t code = 1000;
                    const char *reason = "";
                    if (frame.payload_len >= 2) {
                        code = ((uint16_t)frame.payload[0] << 8) | frame.payload[1];
                    }
                    if (frame.payload_len > 2) {
                        reason = (const char *)frame.payload + 2;
                    }
                    ws_close(client, code, reason);
                }
                return WS_ERR_OK;

            case WS_OP_CONTINUATION:
                /* 分帧处理 (简化) */
                break;
        }
    }

    return WS_ERR_OK;
}

/* 获取状态 */
ws_state_t ws_get_state(ws_client_t *client) {
    return client ? client->state : WS_STATE_CLOSED;
}

/* 检查连接 */
bool ws_is_connected(ws_client_t *client) {
    return ws_get_state(client) == WS_STATE_OPEN;
}

/* 设置保活 */
void ws_set_keepalive(ws_client_t *client, uint32_t interval_ms) {
    if (client != NULL) {
        client->keepalive_interval_ms = interval_ms;
    }
}

/* 错误码转字符串 */
const char* ws_err_str(ws_error_t err) {
    switch (err) {
        case WS_ERR_OK: return "OK";
        case WS_ERR_INVALID_PARAM: return "Invalid parameter";
        case WS_ERR_NO_MEMORY: return "No memory";
        case WS_ERR_CONNECT_FAIL: return "Connection failed";
        case WS_ERR_HANDSHAKE_FAIL: return "Handshake failed";
        case WS_ERR_SEND_FAIL: return "Send failed";
        case WS_ERR_RECV_FAIL: return "Receive failed";
        case WS_ERR_TIMEOUT: return "Timeout";
        case WS_ERR_PROTOCOL: return "Protocol error";
        case WS_ERR_NOT_CONNECTED: return "Not connected";
        default: return "Unknown error";
    }
}
