/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\POSIX_1_2024\01_POSIX_System_Programming.md
 * Line: 3688
 * Language: c
 * Block ID: c81c1d8c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file posix_socket_udp.c
 * @brief POSIX UDP Socket编程
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
#include <errno.h>

#define UDP_PORT 9999
#define BUFFER_SIZE 1024

/**
 * @brief UDP服务器
 */
void udp_server_demo(void) {
    int sock_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len;
    char buffer[BUFFER_SIZE];

    printf("\n=== UDP服务器 ===\n");

    /* 创建UDP socket */
    sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_fd == -1) {
        perror("socket");
        return;
    }

    /* 绑定地址 */
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(UDP_PORT);

    if (bind(sock_fd, (struct sockaddr *)&server_addr,
             sizeof(server_addr)) == -1) {
        perror("bind");
        close(sock_fd);
        return;
    }

    printf("UDP服务器监听端口 %d...\n", UDP_PORT);
    printf("等待数据（2秒超时）...\n");

    /* 设置接收超时 */
    struct timeval timeout;
    timeout.tv_sec = 2;
    timeout.tv_usec = 0;
    setsockopt(sock_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    addr_len = sizeof(client_addr);
    int bytes = recvfrom(sock_fd, buffer, BUFFER_SIZE - 1, 0,
                         (struct sockaddr *)&client_addr, &addr_len);

    if (bytes == -1) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            printf("超时：没有收到数据\n");
        } else {
            perror("recvfrom");
        }
    } else {
        buffer[bytes] = '\0';
        printf("从 %s:%d 收到: %s\n",
               inet_ntoa(client_addr.sin_addr),
               ntohs(client_addr.sin_port),
               buffer);

        /* 发送响应 */
        const char *response = "UDP response from server";
        sendto(sock_fd, response, strlen(response), 0,
               (struct sockaddr *)&client_addr, addr_len);
    }

    close(sock_fd);
}

/**
 * @brief UDP客户端
 */
void udp_client_demo(void) {
    int sock_fd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    socklen_t addr_len;

    printf("\n=== UDP客户端 ===\n");

    sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_fd == -1) {
        perror("socket");
        return;
    }

    /* 设置服务器地址 */
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(UDP_PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    /* 发送数据 */
    const char *message = "Hello UDP server!";
    sendto(sock_fd, message, strlen(message), 0,
           (struct sockaddr *)&server_addr, sizeof(server_addr));
    printf("发送: %s\n", message);

    /* 接收响应 */
    addr_len = sizeof(server_addr);
    struct timeval timeout = {2, 0};
    setsockopt(sock_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    int bytes = recvfrom(sock_fd, buffer, BUFFER_SIZE - 1, 0,
                         (struct sockaddr *)&server_addr, &addr_len);

    if (bytes > 0) {
        buffer[bytes] = '\0';
        printf("收到响应: %s\n", buffer);
    } else {
        printf("未收到响应（服务器可能未运行）\n");
    }

    close(sock_fd);
}

/**
 * @brief UDP广播
 */
void udp_broadcast_demo(void) {
    int sock_fd;
    int broadcast_enable = 1;
    struct sockaddr_in broadcast_addr;

    printf("\n=== UDP广播 ===\n");

    sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_fd == -1) {
        perror("socket");
        return;
    }

    /* 启用广播 */
    if (setsockopt(sock_fd, SOL_SOCKET, SO_BROADCAST,
                   &broadcast_enable, sizeof(broadcast_enable)) == -1) {
        perror("setsockopt");
        close(sock_fd);
        return;
    }

    /* 设置广播地址 */
    memset(&broadcast_addr, 0, sizeof(broadcast_addr));
    broadcast_addr.sin_family = AF_INET;
    broadcast_addr.sin_port = htons(UDP_PORT);
    broadcast_addr.sin_addr.s_addr = inet_addr("255.255.255.255");

    const char *message = "Broadcast message!";
    int ret = sendto(sock_fd, message, strlen(message), 0,
                     (struct sockaddr *)&broadcast_addr,
                     sizeof(broadcast_addr));

    if (ret == -1) {
        perror("sendto");
    } else {
        printf("广播消息已发送: %s\n", message);
    }

    close(sock_fd);
}

/**
 * @brief Unix域套接字（本地套接字）
 */
#include <sys/un.h>

#define SOCKET_PATH "/tmp/posix_unix_socket"

void unix_domain_socket_demo(void) {
    int server_fd, client_fd;
    struct sockaddr_un server_addr;
    char buffer[BUFFER_SIZE];

    printf("\n=== Unix域套接字 ===\n");

    /* 删除旧socket文件 */
    unlink(SOCKET_PATH);

    /* 创建Unix域socket */
    server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket");
        return;
    }

    /* 绑定到文件系统路径 */
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);

    if (bind(server_fd, (struct sockaddr *)&server_addr,
             sizeof(server_addr)) == -1) {
        perror("bind");
        close(server_fd);
        return;
    }

    listen(server_fd, 5);
    printf("Unix域服务器监听 %s\n", SOCKET_PATH);
    printf("（演示模式，不等待连接）\n");

    close(server_fd);
    unlink(SOCKET_PATH);
}

int main(void) {
    printf("=== POSIX UDP Socket编程演示 ===\n");

    udp_server_demo();
    /* udp_client_demo(); */
    udp_broadcast_demo();
    unix_domain_socket_demo();

    printf("\n=== UDP Socket演示完成 ===\n");
    return 0;
}
