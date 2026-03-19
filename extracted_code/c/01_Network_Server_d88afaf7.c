/*
 * Auto-generated from: 06_Thinking_Representation\04_Case_Studies\01_Network_Server.md
 * Line: 179
 * Language: c
 * Block ID: d88afaf7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

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
