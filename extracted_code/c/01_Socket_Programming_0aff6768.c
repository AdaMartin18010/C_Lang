/*
 * Auto-generated from: 03_System_Technology_Domains\15_Network_Programming\01_Socket_Programming.md
 * Line: 3733
 * Language: c
 * Block ID: 0aff6768
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * tcp_optimize.c - TCP优化参数设置
 */
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <netinet/in.h>

void optimize_tcp_socket(int sockfd) {
    int opt;

    // 1. TCP_NODELAY - 禁用Nagle算法,降低延迟
    // 适用: 交互式应用、游戏、高频交易
    opt = 1;
    setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt));

    // 2. TCP_QUICKACK - 立即发送ACK,不延迟
    // 适用: 需要快速响应的场景
    opt = 1;
    setsockopt(sockfd, IPPROTO_TCP, TCP_QUICKACK, &opt, sizeof(opt));

    // 3. TCP_CORK - 合并小数据包(与NODELAY互斥)
    // 适用: 批量数据传输,如文件上传
    // opt = 1;
    // setsockopt(sockfd, IPPROTO_TCP, TCP_CORK, &opt, sizeof(opt));

    // 4. SO_RCVBUF/SO_SNDBUF - 增大缓冲区
    // 注意: 需要在listen/connect之前设置
    int recvbuf = 256 * 1024;  // 256KB
    int sendbuf = 256 * 1024;
    setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &recvbuf, sizeof(recvbuf));
    setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, &sendbuf, sizeof(sendbuf));

    // 5. TCP_KEEPALIVE - 连接保活
    opt = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, &opt, sizeof(opt));

    int idle = 60;      // 60秒无数据开始探测
    int interval = 10;  // 10秒间隔
    int count = 3;      // 3次无响应断开
    setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPIDLE, &idle, sizeof(idle));
    setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPINTVL, &interval, sizeof(interval));
    setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPCNT, &count, sizeof(count));

    // 6. TCP_USER_TIMEOUT - 用户态超时(应用层控制)
    unsigned int timeout = 20000;  // 20秒
    setsockopt(sockfd, IPPROTO_TCP, TCP_USER_TIMEOUT, &timeout, sizeof(timeout));

    // 7. SO_REUSEPORT - 端口复用(负载均衡)
    opt = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));

    // 8. TCP_FASTOPEN - 快速打开
    opt = 5;  // TFO队列长度
    setsockopt(sockfd, SOL_TCP, TCP_FASTOPEN, &opt, sizeof(opt));
}

void optimize_listen_socket(int listenfd) {
    // 设置监听队列长度
    // listen(listenfd, SOMAXCONN);  // 使用系统最大值
    listen(listenfd, 65535);  // 或显式设置

    // SO_REUSEADDR必须在bind之前设置
    int opt = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
}
