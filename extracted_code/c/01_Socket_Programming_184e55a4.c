/*
 * Auto-generated from: 03_System_Technology_Domains\15_Network_Programming\01_Socket_Programming.md
 * Line: 1413
 * Language: c
 * Block ID: 184e55a4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * tcp_server_thread.c - 多线程并发服务器
 * 使用线程池或每个客户端一个线程
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#define LISTEN_BACKLOG 128
#define BUFFER_SIZE 4096
#define MAX_THREADS 100

// 线程参数结构
typedef struct {
    int client_fd;
    struct sockaddr_in client_addr;
} client_info_t;

// 线程处理函数
void *client_thread(void *arg) {
    client_info_t *info = (client_info_t *)arg;
    int client_fd = info->client_fd;
    struct sockaddr_in client_addr = info->client_addr;
    free(info); // 释放参数内存

    // 分离线程,结束时自动回收资源
    pthread_detach(pthread_self());

    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
    printf("[Thread %lu] Handling client %s:%d\n",
           pthread_self(), client_ip, ntohs(client_addr.sin_port));

    char buffer[BUFFER_SIZE];
    ssize_t n;

    while ((n = recv(client_fd, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[n] = '\0';
        printf("[Thread %lu] Received: %s", pthread_self(), buffer);
        send(client_fd, buffer, n, 0);
    }

    if (n < 0) {
        perror("recv");
    }

    printf("[Thread %lu] Client disconnected\n", pthread_self());
    close(client_fd);
    return NULL;
}

int tcp_server_thread(int port) {
    // 忽略SIGPIPE
    signal(SIGPIPE, SIG_IGN);

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

    printf("Thread server listening on port %d...\n", port);

    int thread_count = 0;

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t addr_len = sizeof(client_addr);

        int client_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &addr_len);
        if (client_fd < 0) {
            if (errno == EINTR) continue;
            perror("accept");
            continue;
        }

        // 限制并发线程数
        if (thread_count >= MAX_THREADS) {
            fprintf(stderr, "Too many connections, rejecting...\n");
            close(client_fd);
            continue;
        }

        // 创建线程参数
        client_info_t *info = malloc(sizeof(client_info_t));
        info->client_fd = client_fd;
        info->client_addr = client_addr;

        pthread_t tid;
        if (pthread_create(&tid, NULL, client_thread, info) != 0) {
            perror("pthread_create");
            free(info);
            close(client_fd);
            continue;
        }

        thread_count++;
    }

    close(listen_fd);
    return 0;
}
