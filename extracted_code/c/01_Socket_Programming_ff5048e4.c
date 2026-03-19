/*
 * Auto-generated from: 03_System_Technology_Domains\15_Network_Programming\01_Socket_Programming.md
 * Line: 3864
 * Language: c
 * Block ID: ff5048e4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * high_perf_server.c - 高并发服务器优化技巧
 */

// 1. 使用SO_REUSEPORT实现内核级负载均衡
void setup_reuseport(int listen_fd, int num_workers) {
    int opt = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));

    // 每个工作进程创建自己的listen socket
    // 内核自动分配新连接到不同进程
}

// 2. 使用accept4节省一次系统调用
#include <sys/syscall.h>

int optimized_accept(int listen_fd) {
    // accept4可以一次性设置SOCK_NONBLOCK和SOCK_CLOEXEC
    int client_fd = accept4(listen_fd, NULL, NULL,
                            SOCK_NONBLOCK | SOCK_CLOEXEC);
    return client_fd;
}

// 3. 使用mmap优化缓冲区分配
#include <sys/mman.h>

void *allocate_buffer_pool(size_t size) {
    // 使用mmap大页内存减少TLB miss
    void *pool = mmap(NULL, size, PROT_READ | PROT_WRITE,
                      MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB, -1, 0);
    if (pool == MAP_FAILED) {
        // 回退到普通malloc
        pool = malloc(size);
    }
    return pool;
}

// 4. 使用批处理减少系统调用
void batch_recv(int epoll_fd, struct epoll_event *events, int maxevents) {
    // 一次性获取所有就绪事件
    int nfds = epoll_wait(epoll_fd, events, maxevents, 0);  // 非阻塞

    // 批量处理读事件
    for (int i = 0; i < nfds; i++) {
        if (events[i].events & EPOLLIN) {
            // 批量读取
            int fd = events[i].data.fd;
            // 尽可能多读,直到EAGAIN
            while (1) {
                char buffer[8192];
                ssize_t n = recv(fd, buffer, sizeof(buffer), 0);
                if (n < 0) {
                    if (errno == EAGAIN) break;
                    // 错误处理
                }
                // 处理数据
            }
        }
    }
}

// 5. 使用环形缓冲区减少拷贝
typedef struct {
    char *buffer;
    size_t size;
    size_t head;
    size_t tail;
} ring_buffer_t;

int ring_buffer_write(ring_buffer_t *rb, const char *data, size_t len) {
    size_t available = (rb->head - rb->tail - 1) & (rb->size - 1);
    if (len > available) return -1;  // 空间不足

    for (size_t i = 0; i < len; i++) {
        rb->buffer[rb->head] = data[i];
        rb->head = (rb->head + 1) & (rb->size - 1);
    }
    return 0;
}

// 6. CPU亲和性绑定
#include <sched.h>

void bind_to_cpu(int cpu_id) {
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(cpu_id, &cpuset);
    sched_setaffinity(0, sizeof(cpuset), &cpuset);
}

// 7. 使用忙轮询(Busy Polling)降低延迟(Linux 3.11+)
void enable_busy_polling(int sockfd) {
    int busy_poll = 50;      // 忙轮询超时(微秒)
    int busy_budget = 64;    // 每次轮询处理的包数

    setsockopt(sockfd, SOL_SOCKET, SO_BUSY_POLL,
               &busy_poll, sizeof(busy_poll));
    setsockopt(sockfd, SOL_SOCKET, SO_BUSY_POLL_BUDGET,
               &busy_budget, sizeof(busy_budget));
}
