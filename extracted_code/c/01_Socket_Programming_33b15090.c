/*
 * Auto-generated from: 03_System_Technology_Domains\15_Network_Programming\01_Socket_Programming.md
 * Line: 937
 * Language: c
 * Block ID: 33b15090
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * tcp_server_iterative.c - 迭代TCP服务器
 * 处理完一个客户端后才能处理下一个
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 4096
#define LISTEN_BACKLOG 128

void handle_client(int client_fd, struct sockaddr_in *client_addr) {
    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr->sin_addr, client_ip, sizeof(client_ip));
    int client_port = ntohs(client_addr->sin_port);

    printf("[Client %s:%d] Connected\n", client_ip, client_port);

    char buffer[BUFFER_SIZE];
    ssize_t n;

    while ((n = recv(client_fd, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[n] = '\0';
        printf("[Client %s:%d] Received %zd bytes: %s",
               client_ip, client_port, n, buffer);

        // 简单echo服务
        if (send(client_fd, buffer, n, 0) != n) {
            perror("send");
            break;
        }
    }

    if (n < 0) {
        perror("recv");
    }

    printf("[Client %s:%d] Disconnected\n", client_ip, client_port);
    close(client_fd);
}

int tcp_server_iterative(int port) {
    // 忽略SIGPIPE
    signal(SIGPIPE, SIG_IGN);

    // 创建Socket
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0) {
        perror("socket");
        return -1;
    }

    // 设置地址复用
    int opt = 1;
    if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt SO_REUSEADDR");
        close(listen_fd);
        return -1;
    }

    // 绑定地址
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

    // 开始监听
    if (listen(listen_fd, LISTEN_BACKLOG) < 0) {
        perror("listen");
        close(listen_fd);
        return -1;
    }

    printf("Server listening on port %d...\n", port);

    // 迭代处理客户端
    while (1) {
        struct sockaddr_in client_addr;
        socklen_t addr_len = sizeof(client_addr);

        int client_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &addr_len);
        if (client_fd < 0) {
            if (errno == EINTR) {
                continue; // 被信号中断,重试
            }
            perror("accept");
            continue;
        }

        handle_client(client_fd, &client_addr);
        // 处理完才继续accept下一个
    }

    close(listen_fd);
    return 0;
}

int main(int argc, char *argv[]) {
    int port = (argc > 1) ? atoi(argv[1]) : 8080;
    return tcp_server_iterative(port);
}
