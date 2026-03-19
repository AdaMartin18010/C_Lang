/*
 * Auto-generated from: 03_System_Technology_Domains\15_Network_Programming\01_Socket_Programming.md
 * Line: 1289
 * Language: c
 * Block ID: a30f6f95
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * tcp_server_fork.c - 多进程并发服务器
 * 每个客户端一个进程,使用signal处理僵尸进程
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#define LISTEN_BACKLOG 128
#define BUFFER_SIZE 4096

// SIGCHLD信号处理: 收割僵尸进程
void sigchld_handler(int sig) {
    // 使用while循环收割所有已终止的子进程
    // WNOHANG: 非阻塞,没有已终止子进程时立即返回0
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

void handle_client(int client_fd) {
    char buffer[BUFFER_SIZE];
    ssize_t n;

    // 子进程中处理客户端
    while ((n = recv(client_fd, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[n] = '\0';
        printf("[PID %d] Received: %s", getpid(), buffer);

        // Echo回客户端
        send(client_fd, buffer, n, 0);
    }

    close(client_fd);
    exit(0); // 子进程退出
}

int tcp_server_fork(int port) {
    // 设置SIGCHLD处理函数
    struct sigaction sa;
    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART; // 被信号中断的系统调用自动重试
    sigaction(SIGCHLD, &sa, NULL);

    // 忽略SIGPIPE
    signal(SIGPIPE, SIG_IGN);

    // 创建监听Socket
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0) {
        perror("socket");
        return -1;
    }

    // 地址复用
    int opt = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // 绑定
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

    printf("Fork server listening on port %d...\n", port);

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t addr_len = sizeof(client_addr);

        int client_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &addr_len);
        if (client_fd < 0) {
            if (errno == EINTR) continue;
            perror("accept");
            continue;
        }

        printf("New connection from %s:%d\n",
               inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            close(client_fd);
            continue;
        } else if (pid == 0) {
            // 子进程
            close(listen_fd); // 子进程不需要监听Socket
            handle_client(client_fd);
            // handle_client中exit
        } else {
            // 父进程
            close(client_fd); // 父进程关闭已连接的Socket
            // 继续accept下一个
        }
    }

    close(listen_fd);
    return 0;
}
