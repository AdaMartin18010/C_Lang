/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\IoT_Protocols\01_IoT_Protocols_Implementation.md
 * Line: 4385
 * Language: c
 * Block ID: 122c3f8f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file http_client.h
 * @brief 轻量级HTTP REST客户端
 */

#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* HTTP方法 */
typedef enum {
    HTTP_GET = 0,
    HTTP_POST = 1,
    HTTP_PUT = 2,
    HTTP_DELETE = 3,
    HTTP_HEAD = 4,
    HTTP_PATCH = 5
} http_method_t;

/* HTTP版本 */
typedef enum {
    HTTP_1_0 = 0,
    HTTP_1_1 = 1
} http_version_t;

/* 状态码 */
typedef enum {
    HTTP_OK = 200,
    HTTP_CREATED = 201,
    HTTP_ACCEPTED = 202,
    HTTP_NO_CONTENT = 204,
    HTTP_BAD_REQUEST = 400,
    HTTP_UNAUTHORIZED = 401,
    HTTP_FORBIDDEN = 403,
    HTTP_NOT_FOUND = 404,
    HTTP_METHOD_NOT_ALLOWED = 405,
    HTTP_REQUEST_TIMEOUT = 408,
    HTTP_INTERNAL_ERROR = 500,
    HTTP_BAD_GATEWAY = 502,
    HTTP_UNAVAILABLE = 503
} http_status_code_t;

/* 最大配置 */
#define HTTP_MAX_URL_LEN        256
#define HTTP_MAX_HEADER_LEN     1024
#define HTTP_MAX_BODY_LEN       4096
#define HTTP_MAX_HOST_LEN       128
#define HTTP_MAX_PATH_LEN       256
#define HTTP_MAX_HEADERS        16

/* 错误码 */
typedef enum {
    HTTP_ERR_OK = 0,
    HTTP_ERR_INVALID_PARAM = -1,
    HTTP_ERR_NO_MEMORY = -2,
    HTTP_ERR_CONNECT_FAIL = -3,
    HTTP_ERR_SEND_FAIL = -4,
    HTTP_ERR_RECV_FAIL = -5,
    HTTP_ERR_TIMEOUT = -6,
    HTTP_ERR_PARSE_FAIL = -7
} http_error_t;

/* 传输层回调 */
typedef int (*http_connect_t)(void *ctx, const char *host, uint16_t port);
typedef int (*http_send_t)(void *ctx, const uint8_t *data, size_t len);
typedef int (*http_recv_t)(void *ctx, uint8_t *data, size_t max_len, uint32_t timeout_ms);
typedef void (*http_close_t)(void *ctx);

/* HTTP头 */
typedef struct {
    char name[64];
    char value[256];
} http_header_t;

/* HTTP请求 */
typedef struct {
    http_method_t method;
    http_version_t version;
    char host[HTTP_MAX_HOST_LEN];
    uint16_t port;
    char path[HTTP_MAX_PATH_LEN];
    http_header_t headers[HTTP_MAX_HEADERS];
    uint8_t header_count;
    uint8_t body[HTTP_MAX_BODY_LEN];
    size_t body_len;
    char content_type[64];
} http_request_t;

/* HTTP响应 */
typedef struct {
    http_version_t version;
    uint16_t status_code;
    char status_text[32];
    http_header_t headers[HTTP_MAX_HEADERS];
    uint8_t header_count;
    uint8_t body[HTTP_MAX_BODY_LEN];
    size_t body_len;
    bool chunked;
    size_t content_length;
} http_response_t;

/* HTTP客户端 */
typedef struct {
    void *transport_ctx;
    http_connect_t connect;
    http_send_t send;
    http_recv_t recv;
    http_close_t close;

    http_version_t default_version;
    uint32_t timeout_ms;
    bool keep_alive;
    bool connected;

    /* 缓冲区 */
    uint8_t tx_buffer[HTTP_MAX_HEADER_LEN + HTTP_MAX_BODY_LEN];
    uint8_t rx_buffer[HTTP_MAX_HEADER_LEN + HTTP_MAX_BODY_LEN];
} http_client_t;

/* API函数 */

/* 初始化 */
int http_client_init(http_client_t *client, void *transport_ctx,
                      http_connect_t connect, http_send_t send,
                      http_recv_t recv, http_close_t close);

/* 请求构建 */
void http_request_init(http_request_t *req, http_method_t method, const char *url);
int http_request_add_header(http_request_t *req, const char *name, const char *value);
int http_request_set_body(http_request_t *req, const uint8_t *data, size_t len);
int http_request_set_json_body(http_request_t *req, const char *json);

/* 执行请求 */
int http_client_request(http_client_t *client, const http_request_t *req,
                         http_response_t *resp);

/* 便捷函数 */
int http_get(http_client_t *client, const char *url, http_response_t *resp);
int http_post(http_client_t *client, const char *url, const uint8_t *data,
               size_t len, const char *content_type, http_response_t *resp);
int http_post_json(http_client_t *client, const char *url, const char *json,
                    http_response_t *resp);
int http_put(http_client_t *client, const char *url, const uint8_t *data,
              size_t len, http_response_t *resp);
int http_delete(http_client_t *client, const char *url, http_response_t *resp);

/* 响应处理 */
const char* http_get_header(const http_response_t *resp, const char *name);
int http_get_status_code(const http_response_t *resp);
const char* http_get_body(const http_response_t *resp, size_t *len);

/* 解析工具 */
int http_parse_url(const char *url, char *host, size_t host_len,
                    uint16_t *port, char *path, size_t path_len);

/* 错误码转字符串 */
const char* http_err_str(http_error_t err);

#ifdef __cplusplus
}
#endif

#endif /* HTTP_CLIENT_H */
