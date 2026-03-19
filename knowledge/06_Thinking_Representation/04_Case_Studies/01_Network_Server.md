# 网络服务器案例研究


---

## 📑 目录

- [网络服务器案例研究](#网络服务器案例研究)
  - [📑 目录](#-目录)
  - [概述](#概述)
  - [1. 高并发服务器架构](#1-高并发服务器架构)
    - [1.1 并发模型对比](#11-并发模型对比)
    - [1.2 服务器架构演进](#12-服务器架构演进)
  - [2. Reactor和Proactor模式](#2-reactor和proactor模式)
    - [2.1 Reactor模式（反应器）](#21-reactor模式反应器)
    - [2.2 Proactor模式（主动器）](#22-proactor模式主动器)
    - [2.3 模式对比](#23-模式对比)
  - [3. epoll/kqueue/IOCP对比](#3-epollkqueueiocp对比)
    - [3.1 平台I/O多路复用机制](#31-平台io多路复用机制)
    - [3.2 性能特性对比](#32-性能特性对比)
  - [4. 完整的高性能TCP服务器实现](#4-完整的高性能tcp服务器实现)
    - [4.1 服务器头文件和配置](#41-服务器头文件和配置)
    - [4.2 核心服务器实现](#42-核心服务器实现)
    - [4.3 编译和测试](#43-编译和测试)
  - [5. 性能优化要点](#5-性能优化要点)
    - [5.1 关键优化策略](#51-关键优化策略)
    - [5.2 扩展方向](#52-扩展方向)
  - [总结](#总结)


---

## 概述

本章深入分析高性能网络服务器的设计与实现，涵盖并发模型、事件驱动架构和平台特定的I/O多路复用机制。
通过完整的代码示例展示如何构建生产级TCP服务器。

---

## 1. 高并发服务器架构

### 1.1 并发模型对比

| 模型 | 特点 | 适用场景 | 优缺点 |
|------|------|----------|--------|
| **多进程** | 每个连接独立进程 | 计算密集型 | 隔离性好，资源开销大 |
| **多线程** | 每个连接独立线程 | I/O密集型 | 切换开销中等，共享内存 |
| **线程池** | 预创建线程队列 | 高并发连接 | 减少创建开销，需管理 |
| **事件驱动** | 单/多线程+事件循环 | 极高并发 | 资源效率高，编程复杂 |
| **协程** | 用户态轻量线程 | 现代C框架 | 低切换成本，生态依赖 |

### 1.2 服务器架构演进

```text
迭代服务器 → 多进程/多线程 → 线程池 → Reactor/Proactor → 多Reactor
```

**现代高性能服务器核心要素：**

- 非阻塞I/O + I/O多路复用
- 事件驱动的反应器模式
- 零拷贝数据传输
- 内存池管理连接
- 高效的定时器实现

---

## 2. Reactor和Proactor模式

### 2.1 Reactor模式（反应器）

**核心思想：** 同步I/O多路复用 + 事件分发

```text
┌─────────────────────────────────────────┐
│              Reactor                    │
│  ┌─────────────┐    ┌─────────────┐     │
│  │ Handle集    │───▶│ 同步事件    │     │
│  │ (socket)    │    │ 多路分离器  │     │
│  └─────────────┘    └──────┬──────┘     │
│                            │            │
│         ┌──────────────────┘            │
│         ▼                               │
│  ┌─────────────┐    ┌─────────────┐     │
│  │ 事件处理器  │◀───│ 事件循环    │     │
│  │ (回调函数)  │    │ (dispatch)  │     │
│  └─────────────┘    └─────────────┘     │
└─────────────────────────────────────────┘
```

**工作流程：**

1. 注册感兴趣的事件（读/写）到多路分离器
2. 调用`select`/`poll`/`epoll`等待事件发生
3. 事件发生时，分发器调用对应处理器
4. 处理器执行同步I/O操作

### 2.2 Proactor模式（主动器）

**核心思想：** 异步I/O + 完成回调

```text
┌─────────────────────────────────────────┐
│              Proactor                   │
│  ┌─────────────┐    ┌─────────────┐     │
│  │ 异步操作    │───▶│ 异步事件    │     │
│  │ 处理器      │    │ 分离器      │     │
│  └─────────────┘    └──────┬──────┘     │
│                            │            │
│         ┌──────────────────┘            │
│         ▼                               │
│  ┌─────────────┐    ┌─────────────┐     │
│  │ 完成处理    │◀───│ 完成队列    │     │
│  │ 器(回调)    │    │             │     │
│  └─────────────┘    └─────────────┘     │
└─────────────────────────────────────────┘
```

**工作流程：**

1. 提交异步I/O操作请求
2. 操作系统执行I/O（不阻塞应用）
3. I/O完成时通知Proactor
4. 调用完成处理器处理结果

### 2.3 模式对比

| 特性 | Reactor | Proactor |
|------|---------|----------|
| I/O类型 | 同步非阻塞 | 异步 |
| 数据读取 | 应用主动read | 系统完成read |
| 复杂度 | 较低 | 较高 |
| 平台支持 | POSIX/Win | Windows IOCP最佳 |
| 典型实现 | epoll/kqueue | IOCP/AIO |

---

## 3. epoll/kqueue/IOCP对比

### 3.1 平台I/O多路复用机制

```c
/* Linux epoll */
int epfd = epoll_create1(EPOLL_CLOEXEC);
struct epoll_event ev = { .events = EPOLLIN, .data.ptr = conn };
epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);
epoll_wait(epfd, events, maxevents, timeout);

/* BSD/macOS kqueue */
int kq = kqueue();
struct kevent ev;
EV_SET(&ev, fd, EVFILT_READ, EV_ADD, 0, 0, conn);
kevent(kq, &ev, 1, NULL, 0, NULL);
kevent(kq, NULL, 0, events, nevents, &timeout);

/* Windows IOCP */
HANDLE iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
CreateIoCompletionPort((HANDLE)sock, iocp, key, 0);
GetQueuedCompletionStatus(iocp, &bytes, &key, &ov, timeout);
```

### 3.2 性能特性对比

| 特性 | Linux epoll | BSD kqueue | Windows IOCP |
|------|-------------|------------|--------------|
| 触发模式 | LT/ET | 边沿触发 | 完成通知 |
| 时间复杂度 | O(1) | O(1) | O(1) |
| 最大描述符 | 无限制 | 无限制 | 无限制 |
| 跨平台 | Linux专用 | BSD/macOS | Windows |
| 事件类型 | 读/写/错误/挂起 | 文件/进程/信号/AIO | I/O完成 |
| 线程安全 | 是 | 是 | 是 |

---

## 4. 完整的高性能TCP服务器实现

### 4.1 服务器头文件和配置

```c
/* high_perf_server.h - 高性能TCP服务器 */
#ifndef HIGH_PERF_SERVER_H
#define HIGH_PERF_SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <errno.h>
#include <assert.h>

/* 平台检测 */
#ifdef _WIN32
    #define PLATFORM_WINDOWS
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include <windows.h>
    #pragma comment(lib, "ws2_32.lib")
#else
    #include <unistd.h>
    #include <fcntl.h>
    #include <sys/socket.h>
    #include <sys/epoll.h>
    #include <netinet/in.h>
    #include <netinet/tcp.h>
    #include <arpa/inet.h>
    #define PLATFORM_LINUX
#endif

/* 服务器配置 */
#define SERVER_PORT         8080
#define MAX_EVENTS          1024
#define MAX_CONNECTIONS     10000
#define BUFFER_SIZE         4096
#define BACKLOG             128
#define TIMEOUT_MS          30000
#define WORKER_THREADS      4

/* 连接状态 */
typedef enum {
    CONN_STATE_IDLE,
    CONN_STATE_READING,
    CONN_STATE_WRITING,
    CONN_STATE_CLOSING
} conn_state_t;

/* 定时器节点 */
typedef struct timer_node {
    uint64_t expire_time;           /* 过期时间(ms) */
    int conn_fd;                    /* 关联连接 */
    struct timer_node *next;        /* 链表下一个 */
    struct timer_node *prev;        /* 链表上一个 */
} timer_node_t;

/* 定时器管理器 */
typedef struct {
    timer_node_t *head;             /* 链表头 */
    uint64_t current_time;          /* 当前时间 */
} timer_manager_t;

/* 连接结构 */
typedef struct connection {
    int fd;                         /* 套接字描述符 */
    conn_state_t state;             /* 连接状态 */
    uint32_t events;                /* 关注的事件 */
    char recv_buf[BUFFER_SIZE];     /* 接收缓冲区 */
    char send_buf[BUFFER_SIZE];     /* 发送缓冲区 */
    size_t recv_len;                /* 接收数据长度 */
    size_t send_len;                /* 发送数据长度 */
    size_t send_offset;             /* 发送偏移 */
    uint64_t last_active;           /* 最后活跃时间 */
    timer_node_t *timer;            /* 关联定时器 */
    struct connection *next;        /* 连接池链表 */
} connection_t;

/* 连接池 */
typedef struct {
    connection_t *pool;             /* 连接数组 */
    connection_t *free_list;        /* 空闲链表 */
    size_t capacity;                /* 容量 */
    size_t used;                    /* 已使用 */
    pthread_mutex_t lock;           /* 互斥锁 */
} conn_pool_t;

/* 服务器上下文 */
typedef struct {
    int listen_fd;                  /* 监听套接字 */
    int epoll_fd;                   /* epoll实例 */
    conn_pool_t conn_pool;          /* 连接池 */
    timer_manager_t timer_mgr;      /* 定时器管理器 */
    bool running;                   /* 运行标志 */
    uint64_t start_time;            /* 启动时间 */
    uint64_t total_requests;        /* 总请求数 */
    uint64_t total_bytes;           /* 总字节数 */
} server_context_t;

/* 函数声明 */
bool server_init(server_context_t *ctx, int port);
void server_run(server_context_t *ctx);
void server_stop(server_context_t *ctx);
void server_cleanup(server_context_t *ctx);

#endif /* HIGH_PERF_SERVER_H */
```

### 4.2 核心服务器实现

```c
/* high_perf_server.c - 高性能TCP服务器实现 */
#ifdef PLATFORM_WINDOWS
/* Windows平台使用IOCP实现 */
#else

#include "high_perf_server.h"
#include <pthread.h>

/* 获取当前时间(ms) */
static uint64_t get_current_time(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}

/* 设置非阻塞模式 */
static bool set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0) return false;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK) >= 0;
}

/* 设置TCP_NODELAY */
static bool set_tcp_nodelay(int fd) {
    int nodelay = 1;
    return setsockopt(fd, IPPROTO_TCP, TCP_NODELAY,
                      &nodelay, sizeof(nodelay)) == 0;
}

/* 设置SO_REUSEADDR */
static bool set_reuse_addr(int fd) {
    int reuse = 1;
    return setsockopt(fd, SOL_SOCKET, SO_REUSEADDR,
                      &reuse, sizeof(reuse)) == 0;
}

/*==================== 定时器管理 ====================*/

static void timer_init(timer_manager_t *mgr) {
    mgr->head = NULL;
    mgr->current_time = get_current_time();
}

static timer_node_t* timer_add(timer_manager_t *mgr,
                                int conn_fd,
                                uint64_t timeout_ms) {
    timer_node_t *node = malloc(sizeof(timer_node_t));
    if (!node) return NULL;

    node->expire_time = mgr->current_time + timeout_ms;
    node->conn_fd = conn_fd;
    node->prev = NULL;

    /* 按过期时间排序插入 */
    timer_node_t *curr = mgr->head;
    while (curr && curr->expire_time < node->expire_time) {
        curr = curr->next;
    }

    if (curr) {
        node->next = curr;
        node->prev = curr->prev;
        if (curr->prev) {
            curr->prev->next = node;
        } else {
            mgr->head = node;
        }
        curr->prev = node;
    } else {
        node->next = NULL;
        if (mgr->head) {
            timer_node_t *tail = mgr->head;
            while (tail->next) tail = tail->next;
            tail->next = node;
            node->prev = tail;
        } else {
            mgr->head = node;
        }
    }

    return node;
}

static void timer_remove(timer_manager_t *mgr, timer_node_t *node) {
    if (!node) return;

    if (node->prev) {
        node->prev->next = node->next;
    } else {
        mgr->head = node->next;
    }

    if (node->next) {
        node->next->prev = node->prev;
    }

    free(node);
}

static void timer_update(timer_manager_t *mgr,
                         timer_node_t *node,
                         uint64_t timeout_ms) {
    timer_remove(mgr, node);
    timer_add(mgr, node->conn_fd, timeout_ms);
}

/* 处理过期定时器，返回处理的过期数量 */
static int timer_process_expired(timer_manager_t *mgr,
                                  int *expired_fds,
                                  int max_expired) {
    mgr->current_time = get_current_time();
    int count = 0;

    while (mgr->head &&
           mgr->head->expire_time <= mgr->current_time &&
           count < max_expired) {
        expired_fds[count++] = mgr->head->conn_fd;
        timer_node_t *next = mgr->head->next;
        free(mgr->head);
        mgr->head = next;
        if (mgr->head) {
            mgr->head->prev = NULL;
        }
    }

    return count;
}

/*==================== 连接池管理 ====================*/

static bool conn_pool_init(conn_pool_t *pool, size_t capacity) {
    pool->pool = calloc(capacity, sizeof(connection_t));
    if (!pool->pool) return false;

    pool->capacity = capacity;
    pool->used = 0;
    pool->free_list = NULL;

    /* 初始化空闲链表 */
    for (size_t i = 0; i < capacity; i++) {
        pool->pool[i].fd = -1;
        pool->pool[i].next = pool->free_list;
        pool->free_list = &pool->pool[i];
    }

    pthread_mutex_init(&pool->lock, NULL);
    return true;
}

static connection_t* conn_pool_alloc(conn_pool_t *pool) {
    pthread_mutex_lock(&pool->lock);

    if (!pool->free_list) {
        pthread_mutex_unlock(&pool->lock);
        return NULL;
    }

    connection_t *conn = pool->free_list;
    pool->free_list = conn->next;
    pool->used++;

    /* 初始化连接 */
    memset(conn, 0, sizeof(connection_t));
    conn->fd = -1;
    conn->state = CONN_STATE_IDLE;
    conn->last_active = get_current_time();

    pthread_mutex_unlock(&pool->lock);
    return conn;
}

static void conn_pool_free(conn_pool_t *pool, connection_t *conn) {
    if (!conn) return;

    pthread_mutex_lock(&pool->lock);

    if (conn->fd >= 0) {
        close(conn->fd);
        conn->fd = -1;
    }

    conn->state = CONN_STATE_IDLE;
    conn->next = pool->free_list;
    pool->free_list = conn;
    pool->used--;

    pthread_mutex_unlock(&pool->lock);
}

static void conn_pool_cleanup(conn_pool_t *pool) {
    pthread_mutex_lock(&pool->lock);

    for (size_t i = 0; i < pool->capacity; i++) {
        if (pool->pool[i].fd >= 0) {
            close(pool->pool[i].fd);
        }
    }

    free(pool->pool);
    pool->pool = NULL;
    pool->free_list = NULL;

    pthread_mutex_unlock(&pool->lock);
    pthread_mutex_destroy(&pool->lock);
}

/*==================== 网络操作 ====================*/

static bool server_socket_init(server_context_t *ctx, int port) {
    ctx->listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (ctx->listen_fd < 0) {
        perror("socket");
        return false;
    }

    if (!set_nonblocking(ctx->listen_fd) ||
        !set_reuse_addr(ctx->listen_fd) ||
        !set_tcp_nodelay(ctx->listen_fd)) {
        close(ctx->listen_fd);
        return false;
    }

    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(port),
        .sin_addr.s_addr = INADDR_ANY
    };

    if (bind(ctx->listen_fd, (struct sockaddr *)&addr,
             sizeof(addr)) < 0) {
        perror("bind");
        close(ctx->listen_fd);
        return false;
    }

    if (listen(ctx->listen_fd, BACKLOG) < 0) {
        perror("listen");
        close(ctx->listen_fd);
        return false;
    }

    printf("Server listening on port %d\n", port);
    return true;
}

static bool epoll_init(server_context_t *ctx) {
    ctx->epoll_fd = epoll_create1(EPOLL_CLOEXEC);
    if (ctx->epoll_fd < 0) {
        perror("epoll_create1");
        return false;
    }

    struct epoll_event ev = {
        .events = EPOLLIN,
        .data.ptr = NULL  /* NULL表示监听套接字 */
    };

    if (epoll_ctl(ctx->epoll_fd, EPOLL_CTL_ADD,
                  ctx->listen_fd, &ev) < 0) {
        perror("epoll_ctl");
        close(ctx->epoll_fd);
        return false;
    }

    return true;
}

/*==================== 请求处理 ====================*/

/* 简单的HTTP响应生成 */
static void generate_http_response(connection_t *conn,
                                    const char *body) {
    const char *template =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: %zu\r\n"
        "Connection: keep-alive\r\n"
        "\r\n"
        "%s";

    snprintf(conn->send_buf, BUFFER_SIZE,
             template, strlen(body), body);
    conn->send_len = strlen(conn->send_buf);
    conn->send_offset = 0;
}

/* 处理接收到的数据 */
static void handle_request(connection_t *conn,
                           server_context_t *ctx) {
    /* 简单处理：检查是否收到完整HTTP请求 */
    char *end = strstr(conn->recv_buf, "\r\n\r\n");
    if (!end) return;  /* 请求不完整，继续接收 */

    ctx->total_requests++;
    ctx->total_bytes += conn->recv_len;

    /* 生成响应 */
    char body[256];
    snprintf(body, sizeof(body),
             "Hello from High-Perf Server!\n"
             "Request #%llu, Total bytes: %llu\n",
             (unsigned long long)ctx->total_requests,
             (unsigned long long)ctx->total_bytes);

    generate_http_response(conn, body);
    conn->state = CONN_STATE_WRITING;
    conn->recv_len = 0;  /* 清空接收缓冲区 */

    /* 更新关注事件为写 */
    struct epoll_event ev = {
        .events = EPOLLOUT | EPOLLET,
        .data.ptr = conn
    };
    epoll_ctl(ctx->epoll_fd, EPOLL_CTL_MOD, conn->fd, &ev);
}

/* 接受新连接 */
static bool accept_connection(server_context_t *ctx) {
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);

    int client_fd = accept(ctx->listen_fd,
                           (struct sockaddr *)&client_addr,
                           &addr_len);

    if (client_fd < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return true;  /* 没有更多连接 */
        }
        perror("accept");
        return false;
    }

    if (!set_nonblocking(client_fd) || !set_tcp_nodelay(client_fd)) {
        close(client_fd);
        return false;
    }

    connection_t *conn = conn_pool_alloc(&ctx->conn_pool);
    if (!conn) {
        close(client_fd);
        fprintf(stderr, "Connection pool exhausted\n");
        return false;
    }

    conn->fd = client_fd;
    conn->state = CONN_STATE_READING;
    conn->last_active = get_current_time();

    /* 添加定时器 */
    conn->timer = timer_add(&ctx->timer_mgr, client_fd, TIMEOUT_MS);

    /* 添加到epoll */
    struct epoll_event ev = {
        .events = EPOLLIN | EPOLLET,  /* 边沿触发 */
        .data.ptr = conn
    };

    if (epoll_ctl(ctx->epoll_fd, EPOLL_CTL_ADD, client_fd, &ev) < 0) {
        conn_pool_free(&ctx->conn_pool, conn);
        return false;
    }

    printf("New connection: fd=%d, pool used: %zu/%zu\n",
           client_fd, ctx->conn_pool.used, ctx->conn_pool.capacity);

    return true;
}

/* 读取数据 */
static bool handle_read(connection_t *conn, server_context_t *ctx) {
    while (conn->recv_len < BUFFER_SIZE - 1) {
        ssize_t n = recv(conn->fd,
                         conn->recv_buf + conn->recv_len,
                         BUFFER_SIZE - conn->recv_len - 1,
                         0);

        if (n > 0) {
            conn->recv_len += n;
        } else if (n == 0) {
            return false;  /* 连接关闭 */
        } else {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                break;  /* 数据读取完毕 */
            }
            return false;  /* 读取错误 */
        }
    }

    conn->recv_buf[conn->recv_len] = '\0';
    conn->last_active = get_current_time();

    /* 更新定时器 */
    if (conn->timer) {
        timer_remove(&ctx->timer_mgr, conn->timer);
        conn->timer = timer_add(&ctx->timer_mgr, conn->fd, TIMEOUT_MS);
    }

    handle_request(conn, ctx);
    return true;
}

/* 发送数据 */
static bool handle_write(connection_t *conn, server_context_t *ctx) {
    while (conn->send_offset < conn->send_len) {
        ssize_t n = send(conn->fd,
                         conn->send_buf + conn->send_offset,
                         conn->send_len - conn->send_offset,
                         MSG_NOSIGNAL);

        if (n > 0) {
            conn->send_offset += n;
        } else if (n < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                return true;  /* 发送缓冲区满，下次继续 */
            }
            return false;  /* 发送错误 */
        }
    }

    /* 发送完成，切换回读模式 */
    conn->send_len = 0;
    conn->send_offset = 0;
    conn->state = CONN_STATE_READING;

    struct epoll_event ev = {
        .events = EPOLLIN | EPOLLET,
        .data.ptr = conn
    };
    epoll_ctl(ctx->epoll_fd, EPOLL_CTL_MOD, conn->fd, &ev);

    conn->last_active = get_current_time();
    return true;
}

/* 关闭连接 */
static void close_connection(connection_t *conn, server_context_t *ctx) {
    if (conn->timer) {
        timer_remove(&ctx->timer_mgr, conn->timer);
        conn->timer = NULL;
    }

    epoll_ctl(ctx->epoll_fd, EPOLL_CTL_DEL, conn->fd, NULL);
    conn_pool_free(&ctx->conn_pool, conn);

    printf("Connection closed, pool used: %zu/%zu\n",
           ctx->conn_pool.used, ctx->conn_pool.capacity);
}

/*==================== 事件循环 ====================*/

void server_run(server_context_t *ctx) {
    struct epoll_event events[MAX_EVENTS];
    int expired_fds[64];

    ctx->running = true;
    ctx->start_time = get_current_time();

    printf("Server started, entering event loop...\n");

    while (ctx->running) {
        /* 等待事件，100ms超时用于定时器处理 */
        int nfds = epoll_wait(ctx->epoll_fd, events, MAX_EVENTS, 100);

        if (nfds < 0) {
            if (errno == EINTR) continue;
            perror("epoll_wait");
            break;
        }

        /* 处理事件 */
        for (int i = 0; i < nfds; i++) {
            struct epoll_event *ev = &events[i];

            if (ev->data.ptr == NULL) {
                /* 监听套接字有连接 */
                while (accept_connection(ctx)) {}
            } else {
                connection_t *conn = ev->data.ptr;

                if (ev->events & (EPOLLERR | EPOLLHUP)) {
                    close_connection(conn, ctx);
                    continue;
                }

                if (ev->events & EPOLLIN) {
                    if (!handle_read(conn, ctx)) {
                        close_connection(conn, ctx);
                        continue;
                    }
                }

                if (ev->events & EPOLLOUT) {
                    if (!handle_write(conn, ctx)) {
                        close_connection(conn, ctx);
                    }
                }
            }
        }

        /* 处理过期连接 */
        int n_expired = timer_process_expired(&ctx->timer_mgr,
                                               expired_fds, 64);
        for (int i = 0; i < n_expired; i++) {
            /* 查找连接并关闭 */
            connection_t *conn = NULL;
            for (size_t j = 0; j < ctx->conn_pool.capacity; j++) {
                if (ctx->conn_pool.pool[j].fd == expired_fds[i]) {
                    conn = &ctx->conn_pool.pool[j];
                    break;
                }
            }
            if (conn && conn->fd >= 0) {
                printf("Connection timeout: fd=%d\n", conn->fd);
                conn->timer = NULL;  /* 已被timer_process_expired释放 */
                close_connection(conn, ctx);
            }
        }
    }

    printf("Server stopped.\n");
}

/*==================== 生命周期管理 ====================*/

bool server_init(server_context_t *ctx, int port) {
    memset(ctx, 0, sizeof(*ctx));

    if (!server_socket_init(ctx, port)) return false;
    if (!epoll_init(ctx)) return false;
    if (!conn_pool_init(&ctx->conn_pool, MAX_CONNECTIONS)) {
        close(ctx->epoll_fd);
        close(ctx->listen_fd);
        return false;
    }

    timer_init(&ctx->timer_mgr);

    return true;
}

void server_stop(server_context_t *ctx) {
    ctx->running = false;
}

void server_cleanup(server_context_t *ctx) {
    conn_pool_cleanup(&ctx->conn_pool);

    if (ctx->epoll_fd >= 0) {
        close(ctx->epoll_fd);
        ctx->epoll_fd = -1;
    }

    if (ctx->listen_fd >= 0) {
        close(ctx->listen_fd);
        ctx->listen_fd = -1;
    }

    /* 清理定时器 */
    while (ctx->timer_mgr.head) {
        timer_node_t *next = ctx->timer_mgr.head->next;
        free(ctx->timer_mgr.head);
        ctx->timer_mgr.head = next;
    }
}

/*==================== 主函数 ====================*/

int main(int argc, char *argv[]) {
    int port = (argc > 1) ? atoi(argv[1]) : SERVER_PORT;
    server_context_t ctx;

    printf("High-Performance TCP Server\n");
    printf("===========================\n\n");

    if (!server_init(&ctx, port)) {
        fprintf(stderr, "Server initialization failed\n");
        return 1;
    }

    /* 处理SIGINT信号 */
    signal(SIGINT, (void (*)(int))server_stop);

    server_run(&ctx);
    server_cleanup(&ctx);

    return 0;
}

#endif /* PLATFORM_LINUX */
```

### 4.3 编译和测试

```bash
# 编译
gcc -std=c17 -O2 -D_GNU_SOURCE \
    -o high_perf_server \
    high_perf_server.c \
    -lpthread

# 运行
./high_perf_server 8080

# 测试（使用wrk或ab）
wrk -t12 -c400 -d30s http://localhost:8080/
ab -n 100000 -c 1000 http://localhost:8080/
```

---

## 5. 性能优化要点

### 5.1 关键优化策略

| 优化点 | 实现方式 | 效果 |
|--------|----------|------|
| 连接池 | 预分配连接结构 | 减少内存分配开销 |
| 边沿触发 | EPOLLET模式 | 减少epoll调用次数 |
| 零拷贝 | sendfile | 减少数据拷贝 |
| 线程池 | 多Reactor架构 | 利用多核CPU |
| 定时器 | 有序链表+批量处理 | 高效超时管理 |

### 5.2 扩展方向

1. **多线程模型**: 主Reactor+多Worker线程
2. **协议支持**: HTTP/2、WebSocket、gRPC
3. **SSL/TLS**: OpenSSL集成
4. **负载均衡**: 反向代理集成
5. **监控**: 指标收集和日志

---

## 总结

本案例展示了一个完整的Reactor模式高性能TCP服务器实现，包括：

- **事件驱动架构**：基于epoll的Reactor模式
- **连接池管理**：预分配和复用连接资源
- **定时器机制**：有序链表实现高效超时检测
- **完整生命周期**：初始化、运行、清理的全流程

该服务器可作为高性能网络应用的基础框架，支持进一步扩展为多线程模型和更复杂的协议处理。


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
