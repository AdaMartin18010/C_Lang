/*
 * Auto-generated from: 03_System_Technology_Domains\15_Network_Programming\01_Socket_Programming.md
 * Line: 1056
 * Language: c
 * Block ID: ad1f1f99
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * udp_examples.c - UDP编程完整示例
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// ========== UDP客户端 ==========
int udp_client(const char *server_ip, int server_port) {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket");
        return -1;
    }

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(server_port);
    inet_pton(AF_INET, server_ip, &serv_addr.sin_addr);

    // UDP不需要connect,但可以使用connect来固定对端地址
    // connect后的UDP可以使用send/write代替sendto
    // connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

    char sendbuf[1024], recvbuf[1024];
    struct sockaddr_in from_addr;
    socklen_t from_len = sizeof(from_addr);

    while (fgets(sendbuf, sizeof(sendbuf), stdin) != NULL) {
        // 发送数据到服务器
        ssize_t sent = sendto(sockfd, sendbuf, strlen(sendbuf), 0,
                              (struct sockaddr *)&serv_addr, sizeof(serv_addr));
        if (sent < 0) {
            perror("sendto");
            continue;
        }

        // 接收响应(UDP是无连接的,需要记录对端地址)
        ssize_t n = recvfrom(sockfd, recvbuf, sizeof(recvbuf) - 1, 0,
                             (struct sockaddr *)&from_addr, &from_len);
        if (n < 0) {
            perror("recvfrom");
            continue;
        }

        recvbuf[n] = '\0';
        printf("Received from %s:%d: %s",
               inet_ntoa(from_addr.sin_addr), ntohs(from_addr.sin_port), recvbuf);
    }

    close(sockfd);
    return 0;
}

// ========== UDP服务器(迭代) ==========
int udp_server(int port) {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket");
        return -1;
    }

    // 允许地址复用
    int opt = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);

    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("bind");
        close(sockfd);
        return -1;
    }

    printf("UDP server listening on port %d...\n", port);

    char buffer[65535]; // UDP最大包大小
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);

    while (1) {
        ssize_t n = recvfrom(sockfd, buffer, sizeof(buffer) - 1, 0,
                             (struct sockaddr *)&client_addr, &addr_len);
        if (n < 0) {
            perror("recvfrom");
            continue;
        }

        buffer[n] = '\0';
        printf("Received %zd bytes from %s:%d\n", n,
               inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        // Echo回客户端
        sendto(sockfd, buffer, n, 0, (struct sockaddr *)&client_addr, addr_len);
    }

    close(sockfd);
    return 0;
}

// ========== 已连接的UDP客户端 ==========
int udp_connected_client(const char *server_ip, int server_port) {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket");
        return -1;
    }

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(server_port);
    inet_pton(AF_INET, server_ip, &serv_addr.sin_addr);

    // 连接UDP Socket
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("connect");
        close(sockfd);
        return -1;
    }

    // 现在可以使用send/recv代替sendto/recvfrom
    const char *msg = "Hello UDP";
    send(sockfd, msg, strlen(msg), 0);

    char buffer[1024];
    ssize_t n = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
    if (n > 0) {
        buffer[n] = '\0';
        printf("Received: %s\n", buffer);
    }

    close(sockfd);
    return 0;
}

// ========== UDP广播发送 ==========
int udp_broadcast_sender(int broadcast_port) {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket");
        return -1;
    }

    // 启用广播
    int broadcast = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST,
                   &broadcast, sizeof(broadcast)) < 0) {
        perror("setsockopt");
        close(sockfd);
        return -1;
    }

    struct sockaddr_in broadcast_addr;
    memset(&broadcast_addr, 0, sizeof(broadcast_addr));
    broadcast_addr.sin_family = AF_INET;
    broadcast_addr.sin_port = htons(broadcast_port);
    inet_pton(AF_INET, "255.255.255.255", &broadcast_addr.sin_addr);
    // 或使用受限广播: 192.168.1.255 (取决于子网)

    const char *msg = "Broadcast message!";
    sendto(sockfd, msg, strlen(msg), 0,
           (struct sockaddr *)&broadcast_addr, sizeof(broadcast_addr));

    close(sockfd);
    return 0;
}

// ========== UDP多播接收 ==========
int udp_multicast_receiver(int multicast_port, const char *multicast_ip) {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket");
        return -1;
    }

    // 允许地址复用(多播必需)
    int reuse = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

    // 绑定到多播端口
    struct sockaddr_in local_addr;
    memset(&local_addr, 0, sizeof(local_addr));
    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons(multicast_port);
    local_addr.sin_addr.s_addr = INADDR_ANY; // 接收所有接口的多播

    bind(sockfd, (struct sockaddr *)&local_addr, sizeof(local_addr));

    // 加入多播组
    struct ip_mreq mreq;
    mreq.imr_multiaddr.s_addr = inet_addr(multicast_ip);
    mreq.imr_interface.s_addr = INADDR_ANY; // 使用默认接口

    if (setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP,
                   &mreq, sizeof(mreq)) < 0) {
        perror("setsockopt IP_ADD_MEMBERSHIP");
        close(sockfd);
        return -1;
    }

    printf("Joined multicast group %s:%d\n", multicast_ip, multicast_port);

    char buffer[1024];
    while (1) {
        ssize_t n = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
        if (n > 0) {
            buffer[n] = '\0';
            printf("Multicast: %s\n", buffer);
        }
    }

    // 离开多播组(程序退出前)
    setsockopt(sockfd, IPPROTO_IP, IP_DROP_MEMBERSHIP, &mreq, sizeof(mreq));
    close(sockfd);
    return 0;
}
