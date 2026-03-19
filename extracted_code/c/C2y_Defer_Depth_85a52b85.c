/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Defer_Depth.md
 * Line: 1171
 * Language: c
 * Block ID: 85a52b85
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// file: network_example.c
// compile: clang -std=c2y network_example.c -o network_example

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 4096

int http_get(const char *host, int port, const char *path, char **response) {
    printf("Connecting to %s:%d\n", host, port);

    // 创建socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        return -1;
    }
    defer close(sock);

    // 设置服务器地址
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, host, &server_addr.sin_addr) <= 0) {
        fprintf(stderr, "Invalid address: %s\n", host);
        return -1;
    }

    // 连接服务器
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect");
        return -1;
    }
    printf("Connected successfully\n");

    // 构造HTTP请求
    char request[1024];
    snprintf(request, sizeof(request),
             "GET %s HTTP/1.0\r\n"
             "Host: %s\r\n"
             "Connection: close\r\n"
             "\r\n",
             path, host);

    // 发送请求
    if (send(sock, request, strlen(request), 0) < 0) {
        perror("send");
        return -1;
    }
    printf("Request sent\n");

    // 分配响应缓冲区
    char *buffer = malloc(BUFFER_SIZE);
    if (!buffer) {
        perror("malloc");
        return -1;
    }
    defer free(buffer);

    // 接收响应
    size_t total_received = 0;
    ssize_t received;

    while ((received = recv(sock, buffer + total_received,
                           BUFFER_SIZE - total_received - 1, 0)) > 0) {
        total_received += received;
        if (total_received >= BUFFER_SIZE - 1) {
            fprintf(stderr, "Response too large\n");
            return -1;
        }
    }

    if (received < 0) {
        perror("recv");
        return -1;
    }

    buffer[total_received] = '\0';
    printf("Received %zu bytes\n", total_received);

    // 复制响应（跳过defer的buffer）
    *response = strdup(buffer);
    if (!*response) {
        perror("strdup");
        return -1;
    }

    return 0;  // sock和buffer自动清理
}

int main(void) {
    char *response = NULL;

    if (http_get("93.184.216.34", 80, "/", &response) == 0) {
        printf("\n=== Response (first 500 chars) ===\n");
        printf("%.500s...\n", response);
        free(response);
    }

    return 0;
}
