/*
 * Auto-generated from: 03_System_Technology_Domains\15_Network_Programming\01_Socket_Programming.md
 * Line: 2946
 * Language: c
 * Block ID: 6bbfadbb
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * ❌ 问题描述:
 * 多个进程/线程同时阻塞在accept或epoll_wait上
 * 当有新连接到来时,所有阻塞的进程都会被唤醒
 * 但只有其中一个能成功处理,其他返回EAGAIN,浪费CPU
 */

// 错误示例: accept惊群
void bad_thundering_herd() {
    // 多个进程同时accept
    for (int i = 0; i < N; i++) {
        if (fork() == 0) {
            while (1) {
                int client = accept(listen_fd, ...);  // 所有进程同时被唤醒!
                handle_client(client);
            }
        }
    }
}

// ✅ 解决方案1: 使用互斥锁保护accept (accept_mutex)
#include <pthread.h>
pthread_mutex_t accept_mutex = PTHREAD_MUTEX_INITIALIZER;

void good_accept_mutex() {
    while (1) {
        pthread_mutex_lock(&accept_mutex);
        int client = accept(listen_fd, ...);
        pthread_mutex_unlock(&accept_mutex);

        if (client >= 0) {
            handle_client(client);
        }
    }
}

// ✅ 解决方案2: 使用SO_REUSEPORT (Linux 3.9+)
void good_reuseport_solve() {
    // 每个进程创建独立的监听socket
    // 内核自动进行负载均衡,不会出现惊群
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));
    bind(sock, ...);
    listen(sock, ...);
    // 每个进程独立accept,无惊群
}

// ✅ 解决方案3: 单进程accept + 多进程/线程处理
void good_single_acceptor() {
    // 主进程专门accept
    // 将client_fd通过socketpair/unix socket传递给工作进程
    // 或使用线程池
}

// ✅ 解决方案4: epoll使用EPOLLEXCLUSIVE (Linux 4.5+)
void good_epollexclusive() {
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLEXCLUSIVE;  // 只有一个进程被唤醒
    ev.data.fd = listen_fd;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_fd, &ev);
}
