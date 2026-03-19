/*
 * Auto-generated from: 03_System_Technology_Domains\15_Network_Programming\01_Socket_Programming.md
 * Line: 1887
 * Language: c
 * Block ID: 04afc4ab
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * reactor.c - Reactor模式实现
 * 单线程/多线程Reactor模式
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_EVENTS 1024
#define BUFFER_SIZE 8192

// 事件处理器类型
typedef enum {
    EVENT_ACCEPT,   // 接受连接
    EVENT_READ,     // 可读
    EVENT_WRITE,    // 可写
    EVENT_ERROR     // 错误
} event_type_t;

// 事件回调函数原型
typedef void (*event_callback_t)(int fd, event_type_t type, void *arg);

// 事件处理器结构
typedef struct event_handler {
    int fd;
    event_type_t type;
    event_callback_t callback;
    void *arg;
    struct event_handler *next;
} event_handler_t;

// Reactor结构
typedef struct {
    int epoll_fd;
    int listen_fd;
    event_handler_t *handlers;  // 处理器链表
    int running;
} reactor_t;

// 连接上下文
typedef struct {
    int fd;
    char read_buf[BUFFER_SIZE];
    int read_len;
    char write_buf[BUFFER_SIZE];
    int write_len;
} connection_t;

// 创建Reactor
reactor_t *reactor_create() {
    reactor_t *reactor = malloc(sizeof(reactor_t));
    reactor->epoll_fd = epoll_create1(0);
    reactor->handlers = NULL;
    reactor->running = 0;
    return reactor;
}

// 注册事件处理器
void reactor_register(reactor_t *reactor, int fd, event_type_t type,
                      event_callback_t callback, void *arg) {
    event_handler_t *handler = malloc(sizeof(event_handler_t));
    handler->fd = fd;
    handler->type = type;
    handler->callback = callback;
    handler->arg = arg;
    handler->next = reactor->handlers;
    reactor->handlers = handler;

    struct epoll_event ev;
    ev.events = (type == EVENT_READ) ? EPOLLIN :
                (type == EVENT_WRITE) ? EPOLLOUT : EPOLLIN | EPOLLOUT;
    ev.events |= EPOLLET;  // 边缘触发
    ev.data.ptr = handler;
    epoll_ctl(reactor->epoll_fd, EPOLL_CTL_ADD, fd, &ev);
}

// 修改事件
void reactor_modify(reactor_t *reactor, int fd, event_type_t new_type) {
    struct epoll_event ev;
    ev.events = (new_type == EVENT_READ) ? EPOLLIN :
                (new_type == EVENT_WRITE) ? EPOLLOUT : EPOLLIN | EPOLLOUT;
    ev.events |= EPOLLET;
    ev.data.fd = fd;
    epoll_ctl(reactor->epoll_fd, EPOLL_CTL_MOD, fd, &ev);
}

// 事件循环
void reactor_run(reactor_t *reactor) {
    reactor->running = 1;
    struct epoll_event events[MAX_EVENTS];

    while (reactor->running) {
        int nfds = epoll_wait(reactor->epoll_fd, events, MAX_EVENTS, -1);

        for (int i = 0; i < nfds; i++) {
            event_handler_t *handler = events[i].data.ptr;

            if (events[i].events & EPOLLERR || events[i].events & EPOLLHUP) {
                handler->callback(handler->fd, EVENT_ERROR, handler->arg);
                continue;
            }

            if (events[i].events & EPOLLIN) {
                handler->callback(handler->fd, EVENT_READ, handler->arg);
            }

            if (events[i].events & EPOLLOUT) {
                handler->callback(handler->fd, EVENT_WRITE, handler->arg);
            }
        }
    }
}

// 接受连接回调
void on_accept(int listen_fd, event_type_t type, void *arg) {
    reactor_t *reactor = arg;

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t addr_len = sizeof(client_addr);

        int client_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &addr_len);
        if (client_fd < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                break;
            }
            perror("accept");
            return;
        }

        // 设置非阻塞
        int flags = fcntl(client_fd, F_GETFL, 0);
        fcntl(client_fd, F_SETFL, flags | O_NONBLOCK);

        connection_t *conn = calloc(1, sizeof(connection_t));
        conn->fd = client_fd;

        // 注册读事件
        reactor_register(reactor, client_fd, EVENT_READ, NULL, conn);

        printf("New connection: fd=%d\n", client_fd);
    }
}

// 使用示例
int reactor_server(int port) {
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);

    int opt = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);

    bind(listen_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    listen(listen_fd, 128);

    // 设置非阻塞
    fcntl(listen_fd, F_SETFL, O_NONBLOCK);

    reactor_t *reactor = reactor_create();
    reactor->listen_fd = listen_fd;

    // 注册接受连接事件
    // reactor_register(reactor, listen_fd, EVENT_ACCEPT, on_accept, reactor);

    // reactor_run(reactor);

    return 0;
}
