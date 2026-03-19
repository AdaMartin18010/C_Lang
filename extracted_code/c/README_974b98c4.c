/*
 * Auto-generated from: 07_Modern_Toolchain\13_Comprehensive_Examples\README.md
 * Line: 112
 * Language: c
 * Block ID: 974b98c4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* src/server.c - 服务器核心实现 */
#define _POSIX_C_SOURCE 200809L
#include "httpserver/server.h"
#include "httpserver/router.h"
#include "httpserver/request.h"
#include "httpserver/response.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <errno.h>

#define BUFFER_SIZE 8192

struct http_server {
    server_config_t config;
    int listen_fd;
    bool running;
    pthread_t *workers;
    pthread_mutex_t mutex;
    router_t *router;
    http_middleware_t *middlewares;
    uint32_t middleware_count;
};

/* 线程池工作线程 */
static void *worker_thread(void *arg) {
    http_server_t *server = (http_server_t *)arg;

    while (server->running) {
        struct sockaddr_in client_addr;
        socklen_t addr_len = sizeof(client_addr);

        int client_fd = accept(server->listen_fd,
                              (struct sockaddr *)&client_addr,
                              &addr_len);

        if (client_fd < 0) {
            if (errno == EINTR) continue;
            perror("accept");
            continue;
        }

        /* 处理客户端请求 */
        char buffer[BUFFER_SIZE];
        ssize_t received = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);

        if (received > 0) {
            buffer[received] = '\0';

            /* 解析请求 */
            http_request_t *req = http_request_parse(buffer);
            http_response_t *resp = http_response_create();

            if (req && resp) {
                /* 执行中间件链 */
                bool passed = true;
                for (uint32_t i = 0; i < server->middleware_count && passed; i++) {
                    passed = server->middlewares[i](req, resp);
                }

                /* 路由处理 */
                if (passed) {
                    http_handler_t handler = router_find(server->router,
                                                          http_request_method(req),
                                                          http_request_path(req));
                    if (handler) {
                        handler(req, resp);
                    } else {
                        http_response_status(resp, 404);
                        http_response_body(resp, "Not Found");
                    }
                }

                /* 发送响应 */
                char *response_str = http_response_serialize(resp);
                send(client_fd, response_str, strlen(response_str), 0);
                free(response_str);
            }

            http_request_free(req);
            http_response_free(resp);
        }

        close(client_fd);
    }

    return NULL;
}

http_server_t *http_server_create(const server_config_t *config) {
    http_server_t *server = calloc(1, sizeof(http_server_t));
    if (!server) return NULL;

    server->config = *config;
    pthread_mutex_init(&server->mutex, NULL);

    /* 创建路由表 */
    server->router = router_create();

    /* 创建工作线程池 */
    server->workers = calloc(config->thread_pool_size, sizeof(pthread_t));

    /* 创建监听套接字 */
    server->listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server->listen_fd < 0) {
        free(server);
        return NULL;
    }

    /* 设置地址重用 */
    int opt = 1;
    setsockopt(server->listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    /* 绑定地址 */
    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(config->port),
        .sin_addr.s_addr = INADDR_ANY
    };

    if (bind(server->listen_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        close(server->listen_fd);
        free(server);
        return NULL;
    }

    /* 开始监听 */
    if (listen(server->listen_fd, config->backlog) < 0) {
        close(server->listen_fd);
        free(server);
        return NULL;
    }

    printf("HTTP Server listening on port %d\n", config->port);

    return server;
}

int http_server_register_route(http_server_t *server,
                                const char *method,
                                const char *path,
                                http_handler_t handler) {
    if (!server || !method || !path || !handler) return -1;
    return router_register(server->router, method, path, handler);
}

int http_server_start(http_server_t *server) {
    if (!server) return -1;

    server->running = true;

    /* 启动工作线程 */
    for (uint32_t i = 0; i < server->config.thread_pool_size; i++) {
        pthread_create(&server->workers[i], NULL, worker_thread, server);
    }

    return 0;
}

void http_server_stop(http_server_t *server) {
    if (!server) return;

    server->running = false;
    close(server->listen_fd);

    /* 等待工作线程结束 */
    for (uint32_t i = 0; i < server->config.thread_pool_size; i++) {
        pthread_join(server->workers[i], NULL);
    }
}
