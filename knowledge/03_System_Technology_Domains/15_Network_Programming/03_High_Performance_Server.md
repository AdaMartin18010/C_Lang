# 高性能服务器设计与实现

> **层级定位**: 03 System Technology Domains / 15 Network Programming
> **难度级别**: L4-L5
> **预估学习时间**: 10-12 小时

---

## 目录

- [高性能服务器设计与实现](#高性能服务器设计与实现)
  - [目录](#目录)
  - [1. 服务器架构演进](#1-服务器架构演进)
    - [1.1 架构对比](#11-架构对比)
    - [1.2 C10K问题](#12-c10k问题)
  - [2. Reactor模式](#2-reactor模式)
    - [2.1 单线程Reactor](#21-单线程reactor)
    - [2.2 多线程Reactor (One Loop Per Thread)](#22-多线程reactor-one-loop-per-thread)
  - [3. Proactor模式](#3-proactor模式)
    - [3.1 Proactor vs Reactor](#31-proactor-vs-reactor)
    - [3.2 Linux AIO实现](#32-linux-aio实现)
    - [3.3 io\_uring (Linux 5.1+)](#33-io_uring-linux-51)
  - [4. 零拷贝技术](#4-零拷贝技术)
    - [4.1 sendfile](#41-sendfile)
    - [4.2 mmap + sendfile](#42-mmap--sendfile)
  - [5. 性能优化实践](#5-性能优化实践)
    - [5.1 连接优化](#51-连接优化)
    - [5.2 事件循环优化](#52-事件循环优化)
  - [关联导航](#关联导航)
    - [前置知识](#前置知识)
    - [后续延伸](#后续延伸)
    - [参考](#参考)

## 1. 服务器架构演进

### 1.1 架构对比

| 架构 | 连接数 | 延迟 | 复杂度 | 适用场景 |
|:-----|:-------|:-----|:-------|:---------|
| 迭代服务器 | 1 | 高 | 低 | 测试 |
| 多进程 | 低 | 中 | 中 | 计算密集型 |
| 多线程 | 中 | 中 | 中 | 通用 |
| 线程池 | 中 | 低 | 中 | 通用 |
| 事件驱动 | 高 | 低 | 高 | I/O密集型 |
| 多反应堆 | 很高 | 很低 | 很高 | 高并发 |

### 1.2 C10K问题

C10K = Concurrent 10,000 connections

```
问题核心：
- 每个连接创建进程/线程？资源耗尽！
- 解决方案：I/O多路复用 + 事件驱动

关键突破：
1. 非阻塞I/O
2. 事件通知（epoll/kqueue/IOCP）
3. 状态机处理
4. 零拷贝
```

---

## 2. Reactor模式

### 2.1 单线程Reactor

```
┌─────────────────────────────────────┐
│           Reactor (主线程)            │
│  ┌─────────┐    ┌───────────────┐   │
│  │epoll_wait│    │ 事件分发器     │   │
│  │ (阻塞)   │───→│               │   │
│  └─────────┘    │ 读事件 → 读处理器│   │
│       ↑         │ 写事件 → 写处理器│   │
│       └─────────│ 连接事件→接受处理器│   │
│                 └───────────────┘   │
└─────────────────────────────────────┘
```

```c
// 单线程Reactor实现
#include <sys/epoll.h>

#define MAX_EVENTS 1024

typedef struct {
    int fd;
    void (*read_handler)(int fd);
    void (*write_handler)(int fd);
    void *context;
} connection_t;

connection_t *connections[MAX_FD];

void reactor_run(int listen_fd) {
    int epfd = epoll_create1(EPOLL_CLOEXEC);

    // 添加监听fd
    struct epoll_event ev, events[MAX_EVENTS];
    ev.events = EPOLLIN;
    ev.data.ptr = listen_fd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, listen_fd, &ev);

    while (1) {
        int nfds = epoll_wait(epfd, events, MAX_EVENTS, -1);

        for (int i = 0; i < nfds; i++) {
            int fd = events[i].data.fd;

            if (fd == listen_fd) {
                // 新连接
                int connfd = accept(listen_fd, NULL, NULL);
                set_nonblocking(connfd);

                ev.events = EPOLLIN | EPOLLET;  // 边缘触发
                ev.data.fd = connfd;
                epoll_ctl(epfd, EPOLL_CTL_ADD, connfd, &ev);
            } else {
                // 处理事件
                if (events[i].events & EPOLLIN) {
                    handle_read(fd);
                }
                if (events[i].events & EPOLLOUT) {
                    handle_write(fd);
                }
            }
        }
    }
}
```

### 2.2 多线程Reactor (One Loop Per Thread)

```
┌──────────────────────────────────────────────┐
│              主线程 (Acceptor)                │
│          专门接受连接，分发到工作线程            │
│                    │                          │
│         负载均衡策略（轮询/最少连接/哈希）        │
└──────────────────┬───────────────────────────┘
                   │
    ┌──────────────┼──────────────┐
    │              │              │
    ▼              ▼              ▼
┌────────┐   ┌────────┐   ┌────────┐
│SubReactor│   │SubReactor│   │SubReactor│
│Thread 1 │   │Thread 2 │   │Thread N │
│         │   │         │   │         │
│ epoll   │   │ epoll   │   │ epoll   │
│ 处理I/O │   │ 处理I/O │   │ 处理I/O │
└────────┘   └────────┘   └────────┘
```

```c
#include <pthread.h>

typedef struct {
    int epfd;
    pthread_t tid;
    int idx;
} reactor_thread_t;

#define NUM_THREADS 4
static reactor_thread_t reactors[NUM_THREADS];
static int next_idx = 0;

void *reactor_thread(void *arg) {
    reactor_thread_t *rt = arg;
    struct epoll_event events[MAX_EVENTS];

    while (1) {
        int nfds = epoll_wait(rt->epfd, events, MAX_EVENTS, -1);
        for (int i = 0; i < nfds; i++) {
            // 处理事件
        }
    }
    return NULL;
}

void init_reactors() {
    for (int i = 0; i < NUM_THREADS; i++) {
        reactors[i].epfd = epoll_create1(EPOLL_CLOEXEC);
        reactors[i].idx = i;
        pthread_create(&reactors[i].tid, NULL, reactor_thread, &reactors[i]);
    }
}

void accept_and_dispatch(int listen_fd) {
    int connfd = accept4(listen_fd, NULL, NULL, SOCK_NONBLOCK);

    // 轮询选择reactor
    int idx = __atomic_fetch_add(&next_idx, 1, __ATOMIC_RELAXED) % NUM_THREADS;

    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = connfd;
    epoll_ctl(reactors[idx].epfd, EPOLL_CTL_ADD, connfd, &ev);
}
```

---

## 3. Proactor模式

### 3.1 Proactor vs Reactor

```
Reactor: 同步I/O
  事件通知可读 → 应用读取 → 应用处理 → 应用写入

Proactor: 异步I/O
  提交读请求 → 内核读取 → 通知完成 → 应用处理 → 提交写请求
```

### 3.2 Linux AIO实现

```c
#include <libaio.h>
#include <fcntl.h>

typedef struct {
    io_context_t ctx;
    int fd;
} aio_context_t;

void aio_init(aio_context_t *aio, int fd) {
    memset(&aio->ctx, 0, sizeof(aio->ctx));
    io_setup(128, &aio->ctx);
    aio->fd = fd;
}

void aio_submit_read(aio_context_t *aio, void *buf, size_t len, off_t offset) {
    struct iocb iocb;
    struct iocb *iocbs[1] = {&iocb};

    io_prep_pread(&iocb, aio->fd, buf, len, offset);
    iocb.data = aio;  // 回调数据

    io_submit(aio->ctx, 1, iocbs);
}

void aio_poll(aio_context_t *aio) {
    struct io_event events[10];
    struct timespec timeout = {0, 0};

    int n = io_getevents(aio->ctx, 1, 10, events, &timeout);
    for (int i = 0; i < n; i++) {
        // 处理完成的I/O
        struct iocb *iocb = events[i].obj;
        if (events[i].res > 0) {
            // 成功，处理数据
        } else {
            // 错误
        }
    }
}
```

### 3.3 io_uring (Linux 5.1+)

```c
#include <liburing.h>

struct io_uring ring;

void setup_uring() {
    io_uring_queue_init(32, &ring, 0);
}

void submit_read(int fd, void *buf, size_t len, off_t offset) {
    struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
    io_uring_prep_read(sqe, fd, buf, len, offset);
    io_uring_sqe_set_data(sqe, (void *)(uintptr_t)fd);
    io_uring_submit(&ring);
}

void process_completions() {
    struct io_uring_cqe *cqe;
    unsigned head;
    unsigned completed = 0;

    io_uring_for_each_cqe(&ring, head, cqe) {
        if (cqe->res >= 0) {
            // 成功
            int fd = (int)(uintptr_t)io_uring_cqe_get_data(cqe);
        }
        completed++;
    }

    io_uring_cq_advance(&ring, completed);
}
```

---

## 4. 零拷贝技术

### 4.1 sendfile

```c
#include <sys/sendfile.h>

// 传统方式：磁盘 -> 内核 -> 用户缓冲区 -> 内核 -> 网卡
// sendfile：磁盘 -> 内核页缓存 -> 网卡

void send_file_zero_copy(int out_fd, int file_fd, off_t offset, size_t count) {
    off_t off = offset;
    ssize_t sent = sendfile(out_fd, file_fd, &off, count);
}

// splice：管道零拷贝
void splice_example(int fd_in, int fd_out) {
    int pipefd[2];
    pipe(pipefd);

    // fd_in -> pipe
    splice(fd_in, NULL, pipefd[1], NULL, 4096, SPLICE_F_MOVE);

    // pipe -> fd_out
    splice(pipefd[0], NULL, fd_out, NULL, 4096, SPLICE_F_MOVE);

    close(pipefd[0]);
    close(pipefd[1]);
}
```

### 4.2 mmap + sendfile

```c
void *mmap_send(int fd, size_t size) {
    void *addr = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);

    // 直接发送mmap的内存
    write(socket_fd, addr, size);

    munmap(addr, size);
}
```

---

## 5. 性能优化实践

### 5.1 连接优化

```c
// TCP优化设置
void optimize_socket(int fd) {
    int val;

    // 禁用Nagle算法，降低延迟
    val = 1;
    setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &val, sizeof(val));

    // 启用快速打开
    val = 5;
    setsockopt(fd, IPPROTO_TCP, TCP_FASTOPEN, &val, sizeof(val));

    // 调整缓冲区大小
    val = 256 * 1024;
    setsockopt(fd, SOL_SOCKET, SO_SNDBUF, &val, sizeof(val));
    setsockopt(fd, SOL_SOCKET, SO_RCVBUF, &val, sizeof(val));

    // 重用地址和端口
    val = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));
    setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &val, sizeof(val));
}
```

### 5.2 事件循环优化

```c
// 边缘触发(ET)模式
void set_edge_triggered(int epfd, int fd) {
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLOUT | EPOLLET;  // ET模式
    ev.data.fd = fd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);
}

// ET模式必须读到EAGAIN
void handle_read_et(int fd) {
    char buf[4096];
    while (1) {
        ssize_t n = read(fd, buf, sizeof(buf));
        if (n > 0) {
            process_data(buf, n);
        } else if (n == -1 && errno == EAGAIN) {
            break;  // 读完了
        } else {
            close(fd);  // 错误
            break;
        }
    }
}
```

---

## 关联导航

### 前置知识

- [网络编程基础](./readme.md)
- [并发编程](../14_Concurrency_Parallelism/readme.md)
- [性能优化](../../01_Core_Knowledge_System/05_Engineering_Layer/03_Performance_Optimization.md)

### 后续延伸

- [DPDK高性能网络](../01_System_Programming/07_High_Performance_Networking.md)
- [RDMA网络](../13_RDMA_Network/readme.md)

### 参考

- [libevent](https://libevent.org/)
- [libuv](https://libuv.org/)
- [muduo](https://github.com/chenshuo/muduo)
