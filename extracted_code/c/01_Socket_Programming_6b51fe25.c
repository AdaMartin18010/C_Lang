/*
 * Auto-generated from: 03_System_Technology_Domains\15_Network_Programming\01_Socket_Programming.md
 * Line: 731
 * Language: c
 * Block ID: 6b51fe25
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * tcp_client_full.c - 完整的TCP客户端实现
 * 包含: 错误处理、超时设置、优雅关闭
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define BUFFER_SIZE 4096
#define CONNECT_TIMEOUT_SEC 5

// 全局Socket用于信号处理
static volatile int g_sockfd = -1;

void signal_handler(int sig) {
    if (sig == SIGINT && g_sockfd >= 0) {
        close(g_sockfd);
        g_sockfd = -1;
    }
}

/**
 * 设置Socket为非阻塞模式(用于connect超时控制)
 */
int set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0) return -1;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

/**
 * 设置Socket为阻塞模式
 */
int set_blocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0) return -1;
    return fcntl(fd, F_SETFL, flags & ~O_NONBLOCK);
}

/**
 * 带超时的connect
 */
int connect_with_timeout(int sockfd, const struct sockaddr *addr,
                         socklen_t addrlen, int timeout_sec) {
    int flags = fcntl(sockfd, F_GETFL, 0);
    fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);

    int ret = connect(sockfd, addr, addrlen);
    if (ret < 0 && errno != EINPROGRESS) {
        return -1;
    }

    if (ret == 0) {
        // 立即连接成功
        fcntl(sockfd, F_SETFL, flags);
        return 0;
    }

    // 使用select等待连接完成
    fd_set fdset;
    FD_ZERO(&fdset);
    FD_SET(sockfd, &fdset);

    struct timeval tv;
    tv.tv_sec = timeout_sec;
    tv.tv_usec = 0;

    ret = select(sockfd + 1, NULL, &fdset, NULL, &tv);
    if (ret <= 0) {
        return -1; // 超时或错误
    }

    // 检查连接是否成功
    int so_error;
    socklen_t len = sizeof(so_error);
    getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &so_error, &len);

    if (so_error != 0) {
        errno = so_error;
        return -1;
    }

    fcntl(sockfd, F_SETFL, flags);
    return 0;
}

int tcp_client_full(const char *host, int port) {
    // 忽略SIGPIPE信号,避免写入已关闭连接导致进程终止
    signal(SIGPIPE, SIG_IGN);
    signal(SIGINT, signal_handler);

    // 1. 创建Socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        return -1;
    }
    g_sockfd = sockfd;

    // 2. 解析主机名
    struct hostent *server = gethostbyname(host);
    if (server == NULL) {
        fprintf(stderr, "ERROR: Host not found: %s\n", host);
        close(sockfd);
        return -1;
    }

    // 3. 设置服务器地址
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);

    printf("Connecting to %s:%d...\n", inet_ntoa(serv_addr.sin_addr), port);

    // 4. 建立连接(带超时)
    if (connect_with_timeout(sockfd, (struct sockaddr *)&serv_addr,
                             sizeof(serv_addr), CONNECT_TIMEOUT_SEC) < 0) {
        perror("connect");
        close(sockfd);
        return -1;
    }
    printf("Connected successfully!\n");

    // 5. 设置接收超时
    struct timeval tv;
    tv.tv_sec = 30;
    tv.tv_usec = 0;
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    // 6. 数据通信
    char sendbuf[BUFFER_SIZE];
    char recvbuf[BUFFER_SIZE];

    while (1) {
        printf("> ");
        fflush(stdout);

        if (fgets(sendbuf, sizeof(sendbuf), stdin) == NULL) {
            break; // EOF
        }

        // 发送数据
        size_t len = strlen(sendbuf);
        ssize_t sent = send(sockfd, sendbuf, len, 0);
        if (sent < 0) {
            if (errno == EPIPE) {
                printf("Server closed connection\n");
            } else {
                perror("send");
            }
            break;
        }

        // 接收响应
        ssize_t n = recv(sockfd, recvbuf, sizeof(recvbuf) - 1, 0);
        if (n < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                printf("Receive timeout\n");
                continue;
            }
            perror("recv");
            break;
        } else if (n == 0) {
            printf("Server closed connection\n");
            break;
        }

        recvbuf[n] = '\0';
        printf("< %s", recvbuf);
    }

    // 7. 优雅关闭
    printf("Closing connection...\n");
    shutdown(sockfd, SHUT_WR); // 发送FIN

    // 等待对方关闭连接(半关闭状态可以接收剩余数据)
    char buf[1024];
    while (recv(sockfd, buf, sizeof(buf), 0) > 0);

    close(sockfd);
    g_sockfd = -1;
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <host> <port>\n", argv[0]);
        return 1;
    }
    return tcp_client_full(argv[1], atoi(argv[2]));
}
