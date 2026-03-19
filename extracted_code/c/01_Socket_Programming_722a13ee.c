/*
 * Auto-generated from: 03_System_Technology_Domains\15_Network_Programming\01_Socket_Programming.md
 * Line: 1548
 * Language: c
 * Block ID: 722a13ee
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * tcp_server_select.c - select多路复用服务器
 * 单线程处理多个连接
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#define LISTEN_BACKLOG 128
#define BUFFER_SIZE 4096
#define MAX_CLIENTS FD_SETSIZE  // select最大支持1024

typedef struct {
    int fd;
    char buffer[BUFFER_SIZE];
    int buf_len;
} client_t;

client_t clients[MAX_CLIENTS];
int client_count = 0;

int tcp_server_select(int port) {
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

    // 初始化客户端数组
    for (int i = 0; i < MAX_CLIENTS; i++) {
        clients[i].fd = -1;
    }

    printf("Select server listening on port %d...\n", port);

    fd_set read_fds;
    int max_fd = listen_fd;

    while (1) {
        FD_ZERO(&read_fds);
        FD_SET(listen_fd, &read_fds);
        max_fd = listen_fd;

        // 将所有客户端fd加入集合
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clients[i].fd > 0) {
                FD_SET(clients[i].fd, &read_fds);
                if (clients[i].fd > max_fd) {
                    max_fd = clients[i].fd;
                }
            }
        }

        // select等待IO事件
        struct timeval timeout;
        timeout.tv_sec = 1;  // 1秒超时,用于处理定时任务
        timeout.tv_usec = 0;

        int activity = select(max_fd + 1, &read_fds, NULL, NULL, &timeout);
        if (activity < 0) {
            if (errno == EINTR) continue;
            perror("select");
            break;
        }

        if (activity == 0) {
            // 超时,可以处理定时任务
            continue;
        }

        // 新连接
        if (FD_ISSET(listen_fd, &read_fds)) {
            struct sockaddr_in client_addr;
            socklen_t addr_len = sizeof(client_addr);

            int client_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &addr_len);
            if (client_fd < 0) {
                perror("accept");
                continue;
            }

            printf("New connection: %s:%d, fd=%d\n",
                   inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), client_fd);

            // 找到空闲槽位
            int i;
            for (i = 0; i < MAX_CLIENTS; i++) {
                if (clients[i].fd < 0) {
                    clients[i].fd = client_fd;
                    clients[i].buf_len = 0;
                    client_count++;
                    break;
                }
            }

            if (i == MAX_CLIENTS) {
                fprintf(stderr, "Too many clients\n");
                close(client_fd);
            }
        }

        // 处理客户端IO
        for (int i = 0; i < MAX_CLIENTS; i++) {
            int fd = clients[i].fd;
            if (fd < 0) continue;

            if (FD_ISSET(fd, &read_fds)) {
                char recv_buf[BUFFER_SIZE];
                ssize_t n = recv(fd, recv_buf, sizeof(recv_buf), 0);

                if (n <= 0) {
                    // 连接关闭或错误
                    if (n < 0) perror("recv");
                    printf("Client fd=%d disconnected\n", fd);
                    close(fd);
                    clients[i].fd = -1;
                    client_count--;
                } else {
                    // 处理数据(简单echo)
                    send(fd, recv_buf, n, 0);
                }
            }
        }
    }

    close(listen_fd);
    return 0;
}
