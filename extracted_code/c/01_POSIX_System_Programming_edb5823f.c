/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\POSIX_1_2024\01_POSIX_System_Programming.md
 * Line: 3394
 * Language: c
 * Block ID: edb5823f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file posix_socket_tcp.c
 * @brief POSIX TCP Socket编程
 * @description TCP客户端/服务器实现
 */

#define _POSIX_C_SOURCE 202405L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <signal.h>

#define PORT 8888
#define BUFFER_SIZE 1024
#define BACKLOG 10

/**
 * @brief 创建TCP服务器
 */
void tcp_server_demo(void) {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len;
    char buffer[BUFFER_SIZE];
    int opt = 1;

    printf("\n=== TCP服务器 ===\n");

    /* 创建socket */
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket");
        return;
    }

    /* 设置socket选项 */
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR,
                   &opt, sizeof(opt)) == -1) {
        perror("setsockopt");
        close(server_fd);
        return;
    }

    /* 绑定地址 */
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&server_addr,
             sizeof(server_addr)) == -1) {
        perror("bind");
        close(server_fd);
        return;
    }

    /* 监听连接 */
    if (listen(server_fd, BACKLOG) == -1) {
        perror("listen");
        close(server_fd);
        return;
    }

    printf("TCP服务器监听端口 %d...\n", PORT);
    printf("服务器演示：等待5秒客户端连接...\n");

    /* 设置接收超时 */
    struct timeval timeout;
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;
    setsockopt(server_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    client_len = sizeof(client_addr);
    client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);

    if (client_fd == -1) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            printf("超时：没有客户端连接\n");
        } else {
            perror("accept");
        }
        close(server_fd);
        return;
    }

    printf("客户端已连接: %s:%d\n",
           inet_ntoa(client_addr.sin_addr),
           ntohs(client_addr.sin_port));

    /* 接收数据 */
    int bytes_received = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);
    if (bytes_received > 0) {
        buffer[bytes_received] = '\0';
        printf("收到: %s\n", buffer);

        /* 发送响应 */
        const char *response = "Hello from TCP server!";
        send(client_fd, response, strlen(response), 0);
    }

    close(client_fd);
    close(server_fd);
}

/**
 * @brief 创建TCP客户端
 */
void tcp_client_demo(void) {
    int sock_fd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    printf("\n=== TCP客户端 ===\n");

    /* 创建socket */
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd == -1) {
        perror("socket");
        return;
    }

    /* 设置服务器地址 */
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    /* 连接服务器 */
    if (connect(sock_fd, (struct sockaddr *)&server_addr,
                sizeof(server_addr)) == -1) {
        perror("connect");
        close(sock_fd);
        return;
    }

    printf("已连接到服务器\n");

    /* 发送数据 */
    const char *message = "Hello from TCP client!";
    send(sock_fd, message, strlen(message), 0);
    printf("发送: %s\n", message);

    /* 接收响应 */
    int bytes_received = recv(sock_fd, buffer, BUFFER_SIZE - 1, 0);
    if (bytes_received > 0) {
        buffer[bytes_received] = '\0';
        printf("收到响应: %s\n", buffer);
    }

    close(sock_fd);
}

/**
 * @brief 多进程TCP服务器
 */
void tcp_fork_server_demo(void) {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len;
    pid_t pid;

    printf("\n=== 多进程TCP服务器 ===\n");

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket");
        return;
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT + 1);

    if (bind(server_fd, (struct sockaddr *)&server_addr,
             sizeof(server_addr)) == -1) {
        perror("bind");
        close(server_fd);
        return;
    }

    listen(server_fd, BACKLOG);
    printf("多进程服务器监听端口 %d...\n", PORT + 1);
    printf("演示模式：等待3秒...\n");

    /* 设置超时 */
    struct timeval timeout = {3, 0};
    setsockopt(server_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    client_len = sizeof(client_addr);
    client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);

    if (client_fd == -1) {
        printf("没有连接，继续演示...\n");
        close(server_fd);
        return;
    }

    /* 创建子进程处理连接 */
    pid = fork();
    if (pid == 0) {
        /* 子进程 */
        close(server_fd);

        char buffer[BUFFER_SIZE];
        int bytes = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);
        if (bytes > 0) {
            buffer[bytes] = '\0';
            printf("子进程处理: %s\n", buffer);
            send(client_fd, "Processed by child", 18, 0);
        }

        close(client_fd);
        exit(0);
    } else {
        /* 父进程 */
        close(client_fd);
        wait(NULL);
    }

    close(server_fd);
}

/**
 * @brief 使用select的I/O多路复用服务器
 */
void select_server_demo(void) {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len;
    fd_set read_fds, master_fds;
    int max_fd;

    printf("\n=== Select I/O多路复用 ===\n");

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT + 2);

    bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    listen(server_fd, BACKLOG);

    printf("Select服务器监听端口 %d...\n", PORT + 2);
    printf("演示模式，立即退出...\n");

    FD_ZERO(&master_fds);
    FD_SET(server_fd, &master_fds);
    max_fd = server_fd;

    struct timeval timeout;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;

    read_fds = master_fds;
    int ret = select(max_fd + 1, &read_fds, NULL, NULL, &timeout);

    if (ret == 0) {
        printf("Select超时（正常）\n");
    }

    close(server_fd);
}

int main(void) {
    printf("=== POSIX TCP Socket编程演示 ===\n");

    tcp_server_demo();
    /* tcp_client_demo(); */  /* 需要在服务器运行时执行 */
    tcp_fork_server_demo();
    select_server_demo();

    printf("\n=== TCP Socket演示完成 ===\n");
    return 0;
}
