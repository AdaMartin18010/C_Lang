/*
 * Auto-generated from: 03_System_Technology_Domains\15_Network_Programming\01_Socket_Programming.md
 * Line: 3297
 * Language: c
 * Block ID: 1de689cc
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * ❌ 错误示例1: 非线程安全的全局状态
 */
int connection_count = 0;  // 全局变量

void *thread_handler(void *arg) {
    int sock = *(int *)arg;
    connection_count++;  // 非原子操作,数据竞争!
    // ...
    connection_count--;  // 同上
    return NULL;
}

// ✅ 正确做法1: 使用互斥锁
pthread_mutex_t count_mutex = PTHREAD_MUTEX_INITIALIZER;
int connection_count = 0;

void *thread_handler_safe(void *arg) {
    int sock = *(int *)arg;

    pthread_mutex_lock(&count_mutex);
    connection_count++;
    pthread_mutex_unlock(&count_mutex);

    // ...

    pthread_mutex_lock(&count_mutex);
    connection_count--;
    pthread_mutex_unlock(&count_mutex);

    return NULL;
}

// ✅ 正确做法2: 使用原子操作(GCC扩展)
#include <stdatomic.h>
_Atomic int connection_count = 0;

void *thread_handler_atomic(void *arg) {
    atomic_fetch_add(&connection_count, 1);
    // ...
    atomic_fetch_sub(&connection_count, 1);
    return NULL;
}

/**
 * ❌ 错误示例2: 多个线程操作同一个Socket
 */
// 错误: 一个Socket同时被读线程和写线程操作
// 虽然read/write是原子的,但epoll等操作不是

// ✅ 正确做法: Socket分离读/写事件,或使用锁
pthread_mutex_t sock_mutex = PTHREAD_MUTEX_INITIALIZER;

void safe_send(int sock, const void *buf, size_t len) {
    pthread_mutex_lock(&sock_mutex);
    send(sock, buf, len, 0);
    pthread_mutex_unlock(&sock_mutex);
}

/**
 * ❌ 错误示例3: gethostbyname非线程安全
 */
void bad_gethostbyname() {
    // gethostbyname返回静态缓冲区,多线程会冲突
    struct hostent *h = gethostbyname("example.com");
    // h可能被其他线程覆盖!
}

// ✅ 正确做法: 使用线程安全版本
void good_gethostbyname() {
    struct hostent result;
    char buffer[8192];
    struct hostent *h;
    int err;

    gethostbyname_r("example.com", &result, buffer, sizeof(buffer), &h, &err);
    // 或使用 getaddrinfo() - 推荐
}
