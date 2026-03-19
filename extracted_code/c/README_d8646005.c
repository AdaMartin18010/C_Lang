/*
 * Auto-generated from: 07_Modern_Toolchain\13_Comprehensive_Examples\README.md
 * Line: 63
 * Language: c
 * Block ID: d8646005
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* include/httpserver/server.h */
#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include <stdint.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <netinet/in.h>

typedef struct http_server http_server_t;
typedef struct http_request http_request_t;
typedef struct http_response http_response_t;

/* 请求处理函数类型 */
typedef void (*http_handler_t)(const http_request_t *req,
                                http_response_t *resp);

/* 中间件函数类型 */
typedef bool (*http_middleware_t)(const http_request_t *req,
                                   http_response_t *resp);

/* 服务器配置 */
typedef struct {
    uint16_t port;
    int backlog;
    uint32_t thread_pool_size;
    uint32_t max_request_size;
    uint32_t timeout_ms;
} server_config_t;

/* API */
http_server_t *http_server_create(const server_config_t *config);
void http_server_destroy(http_server_t *server);

int http_server_register_route(http_server_t *server,
                                const char *method,
                                const char *path,
                                http_handler_t handler);

int http_server_add_middleware(http_server_t *server,
                                http_middleware_t middleware);

int http_server_start(http_server_t *server);
void http_server_stop(http_server_t *server);

#endif /* HTTP_SERVER_H */
