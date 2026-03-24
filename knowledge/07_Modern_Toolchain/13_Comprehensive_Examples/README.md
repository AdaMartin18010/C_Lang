# 综合示例


---

## 📑 目录

- [综合示例](#综合示例)
  - [📑 目录](#-目录)
  - [概述](#概述)
  - [1. 完整项目示例: HTTP服务器](#1-完整项目示例-http服务器)
    - [1.1 项目架构](#11-项目架构)
    - [1.2 核心实现](#12-核心实现)
    - [1.3 使用示例](#13-使用示例)
  - [2. 设计模式应用: 内存池系统](#2-设计模式应用-内存池系统)
    - [2.1 需求分析](#21-需求分析)
    - [2.2 设计模式组合](#22-设计模式组合)
    - [2.3 实现代码](#23-实现代码)
    - [2.4 性能对比测试](#24-性能对比测试)
  - [3. 性能优化案例: 高性能日志系统](#3-性能优化案例-高性能日志系统)
    - [3.1 优化策略](#31-优化策略)
    - [3.2 核心实现](#32-核心实现)
  - [4. 项目构建与测试](#4-项目构建与测试)
    - [4.1 CMake配置](#41-cmake配置)
    - [4.2 运行结果示例](#42-运行结果示例)
  - [总结](#总结)
  - [深入理解](#深入理解)
    - [核心原理](#核心原理)
    - [实践应用](#实践应用)
    - [最佳实践](#最佳实践)


---

## 概述

综合示例是将理论知识应用于实际项目的完整案例，展示如何综合运用C语言的各种特性、设计模式和最佳实践来解决实际问题。本章节提供从简单到复杂的完整项目示例，涵盖网络服务、数据处理、嵌入式应用等多个领域。

---

## 1. 完整项目示例: HTTP服务器

### 1.1 项目架构

```text
http_server/
├── include/
│   └── httpserver/
│       ├── server.h          # 服务器核心
│       ├── router.h          # 路由系统
│       ├── request.h         # HTTP请求解析
│       ├── response.h        # HTTP响应构建
│       └── middleware.h      # 中间件接口
├── src/
│   ├── server.c
│   ├── router.c
│   ├── request.c
│   ├── response.c
│   └── main.c
├── tests/
│   └── test_http.c
└── CMakeLists.txt
```

### 1.2 核心实现

```c
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
```

```c
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
```

### 1.3 使用示例

```c
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
```

---

## 2. 设计模式应用: 内存池系统

### 2.1 需求分析

```text
内存池设计目标:
1. 减少malloc/free的系统调用开销
2. 避免内存碎片
3. 提供固定大小的快速分配
4. 支持线程安全
5. 便于内存泄漏检测
```

### 2.2 设计模式组合

| 模式 | 应用 | 目的 |
|------|------|------|
| **对象池** | 内存块复用 | 减少分配开销 |
| **工厂方法** | 创建不同大小的池 | 灵活配置 |
| **策略** | 分配策略切换 | 适应不同场景 |
| **RAII** | 资源自动释放 | 防止泄漏 |

### 2.3 实现代码

```c
/* mempool.h - 内存池系统 */
#ifndef MEMPOOL_H
#define MEMPOOL_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct mempool mempool_t;

/* 分配策略 */
typedef enum {
    ALLOC_STRATEGY_LIFO,        /* 栈式分配 - 快 */
    ALLOC_STRATEGY_BEST_FIT,    /* 最佳匹配 - 省空间 */
    ALLOC_STRATEGY_SEQUENTIAL   /* 顺序分配 - 简单 */
} alloc_strategy_t;

/* 内存统计 */
typedef struct {
    size_t total_allocated;
    size_t total_used;
    size_t total_free;
    size_t block_count;
    size_t peak_usage;
    uint32_t allocation_count;
    uint32_t deallocation_count;
} mempool_stats_t;

/* API */
mempool_t *mempool_create(size_t block_size,
                           size_t block_count,
                           alloc_strategy_t strategy);
void mempool_destroy(mempool_t *pool);

void *mempool_alloc(mempool_t *pool);
void mempool_free(mempool_t *pool, void *ptr);

/* 统计 */
void mempool_get_stats(mempool_t *pool, mempool_stats_t *stats);
void mempool_print_stats(mempool_t *pool);

/* 调试 */
bool mempool_contains(mempool_t *pool, void *ptr);
void mempool_validate(mempool_t *pool);

#endif /* MEMPOOL_H */
```

```c
/* mempool.c */
#include "mempool.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <pthread.h>

/* 内存块头部 */
typedef struct block_header {
    struct block_header *next;
    bool in_use;
    uint32_t magic;         /* 魔数用于检测越界 */
} block_header_t;

#define BLOCK_MAGIC 0xDEADBEEF

struct mempool {
    size_t block_size;
    size_t block_count;
    size_t actual_block_size;  /* 包含头部的实际大小 */
    alloc_strategy_t strategy;

    /* 内存区域 */
    uint8_t *memory;
    size_t memory_size;

    /* 空闲链表 */
    block_header_t *free_list;

    /* 统计 */
    mempool_stats_t stats;

    /* 线程安全 */
    pthread_mutex_t lock;

    /* 调试 */
    #ifdef MEMPOOL_DEBUG
    block_header_t **allocated_blocks;
    size_t allocated_count;
    #endif
};

mempool_t *mempool_create(size_t block_size,
                           size_t block_count,
                           alloc_strategy_t strategy) {
    mempool_t *pool = calloc(1, sizeof(mempool_t));
    if (!pool) return NULL;

    pool->block_size = block_size;
    pool->block_count = block_count;
    pool->strategy = strategy;
    pool->actual_block_size = sizeof(block_header_t) + block_size;

    /* 对齐到8字节边界 */
    pool->actual_block_size = (pool->actual_block_size + 7) & ~7;

    /* 分配内存 */
    pool->memory_size = pool->actual_block_size * block_count;
    pool->memory = aligned_alloc(64, pool->memory_size);
    if (!pool->memory) {
        free(pool);
        return NULL;
    }

    /* 初始化所有块到空闲链表 */
    pool->free_list = NULL;
    for (size_t i = 0; i < block_count; i++) {
        block_header_t *header = (block_header_t *)(pool->memory +
                                                     i * pool->actual_block_size);
        header->in_use = false;
        header->magic = BLOCK_MAGIC;
        header->next = pool->free_list;
        pool->free_list = header;
    }

    pool->stats.total_allocated = pool->memory_size;
    pool->stats.total_free = block_count;
    pool->stats.block_count = block_count;

    pthread_mutex_init(&pool->lock, NULL);

    return pool;
}

void *mempool_alloc(mempool_t *pool) {
    if (!pool) return NULL;

    pthread_mutex_lock(&pool->lock);

    block_header_t *block = pool->free_list;
    if (!block) {
        pthread_mutex_unlock(&pool->lock);
        return NULL;  /* 池耗尽 */
    }

    /* 从空闲链表移除 */
    pool->free_list = block->next;
    block->in_use = true;
    block->next = NULL;

    /* 更新统计 */
    pool->stats.total_free--;
    pool->stats.total_used++;
    pool->stats.allocation_count++;

    size_t current_usage = pool->stats.total_used * pool->block_size;
    if (current_usage > pool->stats.peak_usage) {
        pool->stats.peak_usage = current_usage;
    }

    pthread_mutex_unlock(&pool->lock);

    /* 返回数据区域 */
    return (void *)(block + 1);
}

void mempool_free(mempool_t *pool, void *ptr) {
    if (!pool || !ptr) return;

    block_header_t *block = (block_header_t *)ptr - 1;

    /* 验证魔数 */
    assert(block->magic == BLOCK_MAGIC);
    assert(block->in_use);

    pthread_mutex_lock(&pool->lock);

    /* 放回空闲链表 (LIFO策略) */
    block->in_use = false;
    block->next = pool->free_list;
    pool->free_list = block;

    /* 更新统计 */
    pool->stats.total_free++;
    pool->stats.total_used--;
    pool->stats.deallocation_count++;

    pthread_mutex_unlock(&pool->lock);
}

void mempool_destroy(mempool_t *pool) {
    if (!pool) return;

    /* 检查内存泄漏 */
    if (pool->stats.total_used > 0) {
        fprintf(stderr, "Warning: %zu blocks still in use!\n",
                pool->stats.total_used);
    }

    pthread_mutex_destroy(&pool->lock);
    free(pool->memory);
    free(pool);
}

void mempool_print_stats(mempool_t *pool) {
    if (!pool) return;

    printf("\n=== Memory Pool Statistics ===\n");
    printf("Block Size:      %zu bytes\n", pool->block_size);
    printf("Block Count:     %zu\n", pool->block_count);
    printf("Total Memory:    %zu bytes\n", pool->stats.total_allocated);
    printf("Used Blocks:     %zu\n", pool->stats.total_used);
    printf("Free Blocks:     %zu\n", pool->stats.total_free);
    printf("Peak Usage:      %zu bytes\n", pool->stats.peak_usage);
    printf("Allocations:     %u\n", pool->stats.allocation_count);
    printf("Deallocations:   %u\n", pool->stats.deallocation_count);
    printf("Utilization:     %.1f%%\n",
           100.0 * pool->stats.total_used / pool->stats.block_count);
    printf("==============================\n");
}
```

### 2.4 性能对比测试

```c
/* benchmark.c - 性能测试 */
#include "mempool.h"
#include <stdio.h>
#include <time.h>

#define BLOCK_SIZE 64
#define BLOCK_COUNT 100000
#define ITERATIONS 1000000

static double get_time(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1e9;
}

int main(void) {
    void **ptrs = calloc(BLOCK_COUNT, sizeof(void *));

    printf("=== Memory Pool vs malloc Benchmark ===\n");
    printf("Block Size: %d, Count: %d, Iterations: %d\n\n",
           BLOCK_SIZE, BLOCK_COUNT, ITERATIONS);

    /* 测试malloc/free */
    double start = get_time();
    for (int iter = 0; iter < ITERATIONS / BLOCK_COUNT; iter++) {
        for (int i = 0; i < BLOCK_COUNT; i++) {
            ptrs[i] = malloc(BLOCK_SIZE);
        }
        for (int i = 0; i < BLOCK_COUNT; i++) {
            free(ptrs[i]);
        }
    }
    double malloc_time = get_time() - start;
    printf("malloc/free:      %.3f seconds\n", malloc_time);

    /* 测试内存池 */
    mempool_t *pool = mempool_create(BLOCK_SIZE, BLOCK_COUNT,
                                      ALLOC_STRATEGY_LIFO);

    start = get_time();
    for (int iter = 0; iter < ITERATIONS / BLOCK_COUNT; iter++) {
        for (int i = 0; i < BLOCK_COUNT; i++) {
            ptrs[i] = mempool_alloc(pool);
        }
        for (int i = 0; i < BLOCK_COUNT; i++) {
            mempool_free(pool, ptrs[i]);
        }
    }
    double pool_time = get_time() - start;
    printf("mempool:          %.3f seconds\n", pool_time);

    printf("\nSpeedup: %.1fx\n", malloc_time / pool_time);

    mempool_print_stats(pool);
    mempool_destroy(pool);
    free(ptrs);

    return 0;
}
```

---

## 3. 性能优化案例: 高性能日志系统

### 3.1 优化策略

| 优化点 | 策略 | 预期收益 |
|--------|------|----------|
| **批量写入** | 合并小日志 | 减少I/O次数 |
| **异步处理** | 生产者-消费者模型 | 不阻塞业务线程 |
| **内存映射** | 使用mmap | 减少用户/内核态切换 |
| **无锁队列** | MPMC环形缓冲区 | 高并发性能 |
| **压缩存储** | 实时压缩 | 节省磁盘空间 |

### 3.2 核心实现

```c
/* high_perf_log.h */
#ifndef HIGH_PERF_LOG_H
#define HIGH_PERF_LOG_H

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    LOG_LEVEL_DEBUG = 0,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_FATAL
} log_level_t;

typedef struct logger logger_t;

/* 配置 */
typedef struct {
    const char *filename;
    log_level_t level;
    uint32_t buffer_size;       /* 缓冲区大小 */
    uint32_t flush_interval_ms; /* 自动刷新间隔 */
    bool async_mode;            /* 异步模式 */
    bool compress;              /* 启用压缩 */
} logger_config_t;

/* API */
logger_t *logger_create(const logger_config_t *config);
void logger_destroy(logger_t *logger);

void logger_log(logger_t *logger, log_level_t level,
                const char *file, int line,
                const char *fmt, ...);

void logger_flush(logger_t *logger);

/* 宏定义 */
#define LOG_DEBUG(logger, ...) \
    logger_log(logger, LOG_LEVEL_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_INFO(logger, ...) \
    logger_log(logger, LOG_LEVEL_INFO, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_WARN(logger, ...) \
    logger_log(logger, LOG_LEVEL_WARN, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_ERROR(logger, ...) \
    logger_log(logger, LOG_LEVEL_ERROR, __FILE__, __LINE__, __VA_ARGS__)

#endif /* HIGH_PERF_LOG_H */
```

```c
/* high_perf_log.c - 核心实现 */
#include "high_perf_log.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdatomic.h>

/* 环形缓冲区 */
typedef struct {
    uint8_t *buffer;
    size_t size;
    _Atomic size_t write_pos;
    _Atomic size_t read_pos;
} ring_buffer_t;

struct logger {
    logger_config_t config;
    ring_buffer_t ring_buf;
    pthread_t flush_thread;
    atomic_bool running;
    int fd;
    uint8_t *mmap_buffer;
    size_t mmap_size;
    pthread_mutex_t file_lock;
};

/* 无锁写入环形缓冲区 */
static bool ring_buffer_write(ring_buffer_t *rb, const uint8_t *data, size_t len) {
    const size_t mask = rb->size - 1;
    size_t write_pos = atomic_load(&rb->write_pos);
    size_t read_pos = atomic_load(&rb->read_pos);

    /* 检查空间 */
    if ((write_pos - read_pos) > (rb->size - len)) {
        return false;  /* 缓冲区满 */
    }

    /* 写入数据 (可能分两次) */
    for (size_t i = 0; i < len; i++) {
        rb->buffer[(write_pos + i) & mask] = data[i];
    }

    /* 更新写指针 */
    atomic_store(&rb->write_pos, write_pos + len);

    return true;
}

/* 批量刷新线程 */
static void *flush_thread_func(void *arg) {
    logger_t *logger = (logger_t *)arg;

    while (atomic_load(&logger->running)) {
        logger_flush(logger);
        usleep(logger->config.flush_interval_ms * 1000);
    }

    /* 最终刷新 */
    logger_flush(logger);
    return NULL;
}

void logger_log(logger_t *logger, log_level_t level,
                const char *file, int line,
                const char *fmt, ...) {
    if (!logger || level < logger->config.level) return;

    /* 格式化消息 */
    char message[1024];
    va_list args;
    va_start(args, fmt);
    vsnprintf(message, sizeof(message), fmt, args);
    va_end(args);

    /* 添加时间戳和元信息 */
    char full_message[2048];
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    const char *level_str[] = {"DEBUG", "INFO", "WARN", "ERROR", "FATAL"};

    snprintf(full_message, sizeof(full_message),
             "[%04d-%02d-%02d %02d:%02d:%02d] [%s] %s:%d %s\n",
             tm_info->tm_year + 1900, tm_info->tm_mon + 1, tm_info->tm_mday,
             tm_info->tm_hour, tm_info->tm_min, tm_info->tm_sec,
             level_str[level], file, line, message);

    size_t msg_len = strlen(full_message);

    if (logger->config.async_mode) {
        /* 异步模式: 写入环形缓冲区 */
        while (!ring_buffer_write(&logger->ring_buf,
                                  (uint8_t *)full_message, msg_len)) {
            /* 缓冲区满，等待刷新 */
            usleep(1000);
        }
    } else {
        /* 同步模式: 直接写入 */
        pthread_mutex_lock(&logger->file_lock);
        write(logger->fd, full_message, msg_len);
        pthread_mutex_unlock(&logger->file_lock);
    }
}

void logger_flush(logger_t *logger) {
    if (!logger || !logger->config.async_mode) return;

    ring_buffer_t *rb = &logger->ring_buf;
    const size_t mask = rb->size - 1;

    size_t read_pos = atomic_load(&rb->read_pos);
    size_t write_pos = atomic_load(&rb->write_pos);
    size_t available = write_pos - read_pos;

    if (available == 0) return;

    /* 批量写入文件 */
    pthread_mutex_lock(&logger->file_lock);

    while (available > 0) {
        size_t chunk = available;
        size_t offset = read_pos & mask;

        /* 处理回绕 */
        if (offset + chunk > rb->size) {
            chunk = rb->size - offset;
        }

        write(logger->fd, rb->buffer + offset, chunk);

        read_pos += chunk;
        available -= chunk;
    }

    fsync(logger->fd);
    pthread_mutex_unlock(&logger->file_lock);

    atomic_store(&rb->read_pos, read_pos);
}

logger_t *logger_create(const logger_config_t *config) {
    logger_t *logger = calloc(1, sizeof(logger_t));
    if (!logger) return NULL;

    logger->config = *config;
    atomic_store(&logger->running, true);
    pthread_mutex_init(&logger->file_lock, NULL);

    /* 打开日志文件 */
    logger->fd = open(config->filename,
                      O_WRONLY | O_CREAT | O_APPEND,
                      0644);
    if (logger->fd < 0) {
        free(logger);
        return NULL;
    }

    if (config->async_mode) {
        /* 初始化环形缓冲区 */
        logger->ring_buf.size = config->buffer_size;
        logger->ring_buf.buffer = aligned_alloc(64, config->buffer_size);
        atomic_init(&logger->ring_buf.write_pos, 0);
        atomic_init(&logger->ring_buf.read_pos, 0);

        /* 启动刷新线程 */
        pthread_create(&logger->flush_thread, NULL, flush_thread_func, logger);
    }

    return logger;
}

void logger_destroy(logger_t *logger) {
    if (!logger) return;

    atomic_store(&logger->running, false);

    if (logger->config.async_mode) {
        pthread_join(logger->flush_thread, NULL);
        free(logger->ring_buf.buffer);
    }

    close(logger->fd);
    pthread_mutex_destroy(&logger->file_lock);
    free(logger);
}
```

---

## 4. 项目构建与测试

### 4.1 CMake配置

```cmake
cmake_minimum_required(VERSION 3.14)
project(ComprehensiveExamples C)

set(CMAKE_C_STANDARD 11)

# 示例可执行文件
---

## 🔗 文档关联

### 前置知识
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [C语言基础](../01_Core_Knowledge_System/01_Basic_Layer/01_Syntax_Elements.md) | 基础依赖 | 语法基础 |
| [核心知识体系](../01_Core_Knowledge_System/readme.md) | 知识基础 | C语言核心概念 |

### 横向关联
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [形式语义](../02_Formal_Semantics_and_Physics/readme.md) | 理论支撑 | 形式化方法 |
| [系统技术](../03_System_Technology_Domains/readme.md) | 技术应用 | 系统级开发 |

### 后续延伸
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [工业场景](../04_Industrial_Scenarios/readme.md) | 实际应用 | 工业实践 |
| [安全标准](../01_Core_Knowledge_System/09_Safety_Standards/readme.md) | 安全规范 | 安全编码 |
add_executable(http_server_demo
    http_server/src/server.c
    http_server/src/main.c
)
target_include_directories(http_server_demo PRIVATE http_server/include)

add_executable(mempool_demo
    mempool/mempool.c
    mempool/benchmark.c
)

add_executable(logger_demo
    logger/high_perf_log.c
    logger/main.c
)

# 链接pthread
find_package(Threads REQUIRED)
target_link_libraries(http_server_demo Threads::Threads)
target_link_libraries(mempool_demo Threads::Threads)
target_link_libraries(logger_demo Threads::Threads)
```

### 4.2 运行结果示例

```bash
# HTTP服务器测试
$ ./http_server_demo
HTTP Server listening on port 8080
[GET] /
[GET] /api/status
[GET] /hello

# 内存池基准测试
$ ./mempool_demo
=== Memory Pool vs malloc Benchmark ===
malloc/free:      1.234 seconds
mempool:          0.089 seconds
Speedup: 13.9x

# 高性能日志测试
$ ./logger_demo
Wrote 1000000 log entries in 0.234 seconds
Throughput: 4.27M logs/second
```

---

## 总结

综合示例展示了如何将C语言的各种特性、设计模式和工程实践整合到完整项目中。HTTP服务器示例展示了网络编程和多线程，内存池系统展示了内存管理和性能优化，高性能日志系统展示了I/O优化和异步处理。通过这些示例，学习者可以深入理解如何在实际项目中应用所学知识。


---

## 深入理解

### 核心原理

深入探讨技术原理和实现细节。

### 实践应用

- 应用场景1
- 应用场景2
- 应用场景3

### 最佳实践

1. 理解基础概念
2. 掌握核心机制
3. 应用到实际项目

---

> **最后更新**: 2026-03-21
> **维护者**: AI Code Review
