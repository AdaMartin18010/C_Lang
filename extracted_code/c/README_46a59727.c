/*
 * Auto-generated from: 07_Modern_Toolchain\13_Comprehensive_Examples\README.md
 * Line: 290
 * Language: c
 * Block ID: 46a59727
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* src/main.c - HTTP服务器使用示例 */
#include "httpserver/server.h"
#include "httpserver/request.h"
#include "httpserver/response.h"
#include <stdio.h>
#include <signal.h>

static http_server_t *g_server = NULL;

/* 信号处理 */
void signal_handler(int sig) {
    (void)sig;
    printf("\nShutting down server...\n");
    if (g_server) {
        http_server_stop(g_server);
    }
}

/* 处理函数 */
void handle_root(const http_request_t *req, http_response_t *resp) {
    (void)req;
    http_response_status(resp, 200);
    http_response_header(resp, "Content-Type", "text/html");
    http_response_body(resp,
        "<html><body>"
        "<h1>Welcome to C HTTP Server</h1>"
        "<p>Server is running!</p>"
        "</body></html>");
}

void handle_api_status(const http_request_t *req, http_response_t *resp) {
    (void)req;
    http_response_status(resp, 200);
    http_response_header(resp, "Content-Type", "application/json");
    http_response_body(resp, "{\"status\": \"ok\", \"version\": \"1.0\"}");
}

void handle_hello(const http_request_t *req, http_response_t *resp) {
    const char *name = http_request_query(req, "name");
    if (!name) name = "World";

    char body[256];
    snprintf(body, sizeof(body), "<h1>Hello, %s!</h1>", name);

    http_response_status(resp, 200);
    http_response_header(resp, "Content-Type", "text/html");
    http_response_body(resp, body);
}

/* 日志中间件 */
bool logging_middleware(const http_request_t *req, http_response_t *resp) {
    (void)resp;
    printf("[%s] %s\n", http_request_method(req), http_request_path(req));
    return true;
}

int main(void) {
    signal(SIGINT, signal_handler);

    server_config_t config = {
        .port = 8080,
        .backlog = 128,
        .thread_pool_size = 4,
        .max_request_size = 65536,
        .timeout_ms = 30000
    };

    g_server = http_server_create(&config);
    if (!g_server) {
        fprintf(stderr, "Failed to create server\n");
        return 1;
    }

    /* 注册路由 */
    http_server_register_route(g_server, "GET", "/", handle_root);
    http_server_register_route(g_server, "GET", "/api/status", handle_api_status);
    http_server_register_route(g_server, "GET", "/hello", handle_hello);

    /* 添加中间件 */
    http_server_add_middleware(g_server, logging_middleware);

    /* 启动服务器 */
    http_server_start(g_server);

    return 0;
}
