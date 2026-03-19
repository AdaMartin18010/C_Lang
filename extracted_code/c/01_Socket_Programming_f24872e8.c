/*
 * Auto-generated from: 03_System_Technology_Domains\15_Network_Programming\01_Socket_Programming.md
 * Line: 1708
 * Language: c
 * Block ID: f24872e8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * tcp_server_epoll.c - epoll高性能服务器
 * 支持Level Trigger和Edge Trigger模式
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

#define LISTEN_BACKLOG 128
#define MAX_EVENTS 1024
#define BUFFER_SIZE 4096

// 设置非阻塞
int set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

// LT模式处理客户端读
void handle_read_lt(int epoll_fd, int client_fd) {
    char buffer[BUFFER_SIZE];
    ssize_t n = recv(client_fd, buffer, sizeof(buffer) - 1, 0);

    if (n <= 0) {
        if (n < 0) perror("recv");
        epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
        close(client_fd);
        printf("Client fd=%d closed (LT)\n", client_fd);
        return;
    }

    buffer[n] = '\0';
    printf("[LT] Received %zd bytes from fd=%d\n", n, client_fd);

    // Echo回客户端
    send(client_fd, buffer, n, 0);
}

// ET模式处理客户端读 - 必须读到EAGAIN
void handle_read_et(int epoll_fd, int client_fd) {
    char buffer[BUFFER_SIZE];
    ssize_t total = 0;

    // ET模式下必须循环读到返回EAGAIN
    while (1) {
        ssize_t n = recv(client_fd, buffer, sizeof(buffer), 0);

        if (n < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // 数据读完了
                break;
            }
            perror("recv");
            epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
            close(client_fd);
            return;
        } else if (n == 0) {
            // 连接关闭
            epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
            close(client_fd);
            printf("Client fd=%d closed (ET)\n", client_fd);
            return;
        }

        total += n;
        // 处理数据...
        send(client_fd, buffer, n, 0);
    }

    printf("[ET] Total received %zd bytes from fd=%d\n", total, client_fd);
}

int tcp_server_epoll(int port, int use_et) {
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0) {
        perror("socket");
        return -1;
    }

    int opt = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);

    if (bind(listen_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("bind");
        close(listen_fd);
        return -1;
    }

    if (listen(listen_fd, LISTEN_BACKLOG) < 0) {
        perror("listen");
        close(listen_fd);
        return -1;
    }

    // 创建epoll实例
    int epoll_fd = epoll_create1(0);
    if (epoll_fd < 0) {
        perror("epoll_create1");
        close(listen_fd);
        return -1;
    }

    // 添加监听fd到epoll
    struct epoll_event ev, events[MAX_EVENTS];
    ev.events = EPOLLIN;
    ev.data.fd = listen_fd;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_fd, &ev);

    printf("Epoll server listening on port %d (%s mode)...\n",
           port, use_et ? "ET" : "LT");

    while (1) {
        int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if (nfds < 0) {
            if (errno == EINTR) continue;
            perror("epoll_wait");
            break;
        }

        for (int i = 0; i < nfds; i++) {
            if (events[i].data.fd == listen_fd) {
                // 新连接
                struct sockaddr_in client_addr;
                socklen_t addr_len = sizeof(client_addr);

                int client_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &addr_len);
                if (client_fd < 0) {
                    perror("accept");
                    continue;
                }

                // ET模式需要设置非阻塞
                if (use_et) {
                    set_nonblocking(client_fd);
                }

                // 添加新连接到epoll
                ev.data.fd = client_fd;
                ev.events = EPOLLIN | (use_et ? EPOLLET : 0);
                epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &ev);

                printf("New connection: %s:%d, fd=%d\n",
                       inet_ntoa(client_addr.sin_addr),
                       ntohs(client_addr.sin_port), client_fd);
            } else {
                // 客户端数据
                if (use_et) {
                    handle_read_et(epoll_fd, events[i].data.fd);
                } else {
                    handle_read_lt(epoll_fd, events[i].data.fd);
                }
            }
        }
    }

    close(epoll_fd);
    close(listen_fd);
    return 0;
}
