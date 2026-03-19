/*
 * Auto-generated from: 03_System_Technology_Domains\15_Network_Programming\01_Socket_Programming.md
 * Line: 2239
 * Language: c
 * Block ID: 933a3418
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * nonblocking_io.c - 非阻塞IO实现示例
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// 设置非阻塞模式
int set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0) return -1;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

// 非阻塞Connect
int nonblocking_connect(const char *ip, int port, int timeout_sec) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) return -1;

    set_nonblocking(sockfd);

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, ip, &addr.sin_addr);

    int ret = connect(sockfd, (struct sockaddr *)&addr, sizeof(addr));
    if (ret < 0 && errno != EINPROGRESS) {
        close(sockfd);
        return -1;
    }

    if (ret == 0) {
        // 立即连接成功
        return sockfd;
    }

    // 等待连接完成(使用select)
    fd_set fdset;
    FD_ZERO(&fdset);
    FD_SET(sockfd, &fdset);

    struct timeval tv;
    tv.tv_sec = timeout_sec;
    tv.tv_usec = 0;

    ret = select(sockfd + 1, NULL, &fdset, NULL, &tv);
    if (ret <= 0) {
        close(sockfd);
        return -1;
    }

    // 检查连接状态
    int so_error;
    socklen_t len = sizeof(so_error);
    getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &so_error, &len);

    if (so_error != 0) {
        errno = so_error;
        close(sockfd);
        return -1;
    }

    return sockfd;
}

// 非阻塞发送(处理EAGAIN)
ssize_t nonblocking_send(int fd, const void *buf, size_t len) {
    const char *p = buf;
    size_t remaining = len;

    while (remaining > 0) {
        ssize_t n = send(fd, p, remaining, MSG_NOSIGNAL);

        if (n > 0) {
            p += n;
            remaining -= n;
        } else if (n < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // 发送缓冲区已满,需要等待
                return len - remaining; // 返回已发送字节数
            } else if (errno == EINTR) {
                continue; // 被信号中断,重试
            } else {
                return -1; // 错误
            }
        } else {
            return -1; // 连接关闭
        }
    }

    return len;
}

// 非阻塞接收(处理EAGAIN)
ssize_t nonblocking_recv(int fd, void *buf, size_t len) {
    ssize_t total = 0;
    char *p = buf;

    while (1) {
        ssize_t n = recv(fd, p + total, len - total, 0);

        if (n > 0) {
            total += n;
            if ((size_t)total >= len) break; // 缓冲区已满
        } else if (n < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // 无数据可读
                break;
            } else if (errno == EINTR) {
                continue;
            } else {
                return (total > 0) ? total : -1;
            }
        } else {
            // n == 0, 连接关闭
            break;
        }
    }

    return total;
}
