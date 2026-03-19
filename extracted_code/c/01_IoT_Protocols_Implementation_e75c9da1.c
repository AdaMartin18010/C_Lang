/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\IoT_Protocols\01_IoT_Protocols_Implementation.md
 * Line: 1367
 * Language: c
 * Block ID: e75c9da1
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file coap_client.h
 * @brief CoAP客户端实现 - 轻量级受限设备版本
 */

#ifndef COAP_CLIENT_H
#define COAP_CLIENT_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* CoAP版本 */
#define COAP_VERSION 1

/* CoAP消息类型 */
typedef enum {
    COAP_TYPE_CON = 0,      /* Confirmable - 需要确认 */
    COAP_TYPE_NON = 1,      /* Non-confirmable - 不需要确认 */
    COAP_TYPE_ACK = 2,      /* Acknowledgment - 确认 */
    COAP_TYPE_RST = 3       /* Reset - 复位 */
} coap_msg_type_t;

/* CoAP请求方法 (Code 0.01-0.31) */
typedef enum {
    COAP_METHOD_GET = 1,    /* 0.01 */
    COAP_METHOD_POST = 2,   /* 0.02 */
    COAP_METHOD_PUT = 3,    /* 0.03 */
    COAP_METHOD_DELETE = 4  /* 0.04 */
} coap_method_t;

/* CoAP响应码 (Code 2.00-5.31) */
typedef enum {
    /* 成功 2.xx */
    COAP_CODE_CREATED = 0x41,           /* 2.01 */
    COAP_CODE_DELETED = 0x42,           /* 2.02 */
    COAP_CODE_VALID = 0x43,             /* 2.03 */
    COAP_CODE_CHANGED = 0x44,           /* 2.04 */
    COAP_CODE_CONTENT = 0x45,           /* 2.05 */

    /* 客户端错误 4.xx */
    COAP_CODE_BAD_REQUEST = 0x80,       /* 4.00 */
    COAP_CODE_UNAUTHORIZED = 0x81,      /* 4.01 */
    COAP_CODE_BAD_OPTION = 0x82,        /* 4.02 */
    COAP_CODE_FORBIDDEN = 0x83,         /* 4.03 */
    COAP_CODE_NOT_FOUND = 0x84,         /* 4.04 */
    COAP_CODE_METHOD_NOT_ALLOWED = 0x85, /* 4.05 */
    COAP_CODE_NOT_ACCEPTABLE = 0x86,    /* 4.06 */
    COAP_CODE_PRECONDITION_FAILED = 0x8C, /* 4.12 */
    COAP_CODE_REQUEST_TOO_LARGE = 0x8D, /* 4.13 */
    COAP_CODE_UNSUPPORTED_FORMAT = 0x8F, /* 4.15 */

    /* 服务器错误 5.xx */
    COAP_CODE_INTERNAL_ERROR = 0xA0,    /* 5.00 */
    COAP_CODE_NOT_IMPLEMENTED = 0xA1,   /* 5.01 */
    COAP_CODE_BAD_GATEWAY = 0xA2,       /* 5.02 */
    COAP_CODE_UNAVAILABLE = 0xA3,       /* 5.03 */
    COAP_CODE_GATEWAY_TIMEOUT = 0xA4,   /* 5.04 */
    COAP_CODE_PROXYING_NOT_SUPPORTED = 0xA5 /* 5.05 */
} coap_response_code_t;

/* CoAP选项 */
typedef enum {
    COAP_OPTION_IF_MATCH = 1,
    COAP_OPTION_URI_HOST = 3,
    COAP_OPTION_ETAG = 4,
    COAP_OPTION_IF_NONE_MATCH = 5,
    COAP_OPTION_URI_PORT = 7,
    COAP_OPTION_LOCATION_PATH = 8,
    COAP_OPTION_URI_PATH = 11,
    COAP_OPTION_CONTENT_FORMAT = 12,
    COAP_OPTION_MAX_AGE = 14,
    COAP_OPTION_URI_QUERY = 15,
    COAP_OPTION_ACCEPT = 17,
    COAP_OPTION_LOCATION_QUERY = 20,
    COAP_OPTION_BLOCK2 = 23,
    COAP_OPTION_BLOCK1 = 27,
    COAP_OPTION_SIZE2 = 28,
    COAP_OPTION_PROXY_URI = 35,
    COAP_OPTION_PROXY_SCHEME = 39,
    COAP_OPTION_SIZE1 = 60
} coap_option_num_t;

/* 内容格式 */
typedef enum {
    COAP_FORMAT_TEXT_PLAIN = 0,
    COAP_FORMAT_LINK_FORMAT = 40,
    COAP_FORMAT_XML = 41,
    COAP_FORMAT_OCTET_STREAM = 42,
    COAP_FORMAT_JSON = 50,
    COAP_FORMAT_CBOR = 60
} coap_content_format_t;

/* 最大配置 */
#define COAP_MAX_PAYLOAD_LEN    1024
#define COAP_MAX_OPTIONS        16
#define COAP_MAX_TOKEN_LEN      8
#define COAP_MAX_URI_LEN        128
#define COAP_MAX_PACKET_LEN     1280

/* 默认端口 */
#define COAP_DEFAULT_PORT       5683
#define COAPS_DEFAULT_PORT      5684

/* 前向声明 */
struct coap_client;
typedef struct coap_client coap_client_t;

/* 网络回调 */
typedef int (*coap_network_send_t)(void *context, const uint8_t *data, size_t len,
                                    const void *addr, size_t addr_len);
typedef int (*coap_network_recv_t)(void *context, uint8_t *data, size_t max_len,
                                    void *addr, size_t *addr_len, uint32_t timeout_ms);

/* 请求回调 */
typedef void (*coap_response_callback_t)(coap_client_t *client, uint8_t code,
                                          const uint8_t *payload, size_t len,
                                          void *user_data);

/* CoAP选项结构 */
typedef struct {
    uint16_t number;
    uint8_t value[32];
    size_t len;
} coap_option_t;

/* CoAP消息结构 */
typedef struct {
    uint8_t version;
    coap_msg_type_t type;
    uint8_t code;
    uint16_t msg_id;
    uint8_t token[COAP_MAX_TOKEN_LEN];
    uint8_t token_len;
    coap_option_t options[COAP_MAX_OPTIONS];
    uint8_t option_count;
    uint8_t payload[COAP_MAX_PAYLOAD_LEN];
    size_t payload_len;
} coap_message_t;

/* 挂起请求 */
typedef struct {
    uint16_t msg_id;
    uint8_t token[COAP_MAX_TOKEN_LEN];
    uint8_t token_len;
    coap_msg_type_t type;
    uint32_t timestamp;
    uint8_t retry_count;
    coap_response_callback_t callback;
    void *user_data;
    bool active;
} coap_pending_request_t;

/* CoAP客户端结构 */
struct coap_client {
    void *network_context;
    coap_network_send_t network_send;
    coap_network_recv_t network_recv;

    /* 挂起请求列表 */
    coap_pending_request_t pending[8];

    /* 消息ID计数器 */
    uint16_t next_msg_id;

    /* 缓冲区 */
    uint8_t tx_buffer[COAP_MAX_PACKET_LEN];
    uint8_t rx_buffer[COAP_MAX_PACKET_LEN];

    /* 重传配置 */
    uint32_t ack_timeout_ms;
    uint8_t max_retransmit;

    /* 观察者列表(简化实现) */
    struct {
        char resource[64];
        uint32_t last_observe_seq;
        bool active;
    } observations[4];
};

/* 函数声明 */
int coap_client_init(coap_client_t *client, void *network_context,
                      coap_network_send_t send_func, coap_network_recv_t recv_func);
int coap_request(coap_client_t *client, coap_method_t method,
                  const char *uri, const uint8_t *payload, size_t payload_len,
                  coap_content_format_t content_type,
                  coap_response_callback_t callback, void *user_data);
int coap_get(coap_client_t *client, const char *uri,
              coap_response_callback_t callback, void *user_data);
int coap_post(coap_client_t *client, const char *uri,
               const uint8_t *payload, size_t len,
               coap_content_format_t format,
               coap_response_callback_t callback, void *user_data);
int coap_put(coap_client_t *client, const char *uri,
              const uint8_t *payload, size_t len,
              coap_content_format_t format,
              coap_response_callback_t callback, void *user_data);
int coap_delete(coap_client_t *client, const char *uri,
                 coap_response_callback_t callback, void *user_data);
int coap_observe(coap_client_t *client, const char *uri,
                  coap_response_callback_t callback, void *user_data);
int coap_cancel_observe(coap_client_t *client, const char *uri);
int coap_process(coap_client_t *client, uint32_t timeout_ms);

/* 辅助函数 */
int coap_build_uri_options(const char *uri, coap_option_t *options, int max_options);
const char* coap_response_code_str(uint8_t code);

#ifdef __cplusplus
}
#endif

#endif /* COAP_CLIENT_H */
