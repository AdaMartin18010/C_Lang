# C 语言网络编程实战指南

> **难度**: L3-L5
> **平台**: Linux (epoll/io_uring), POSIX
> **核心目标**: 掌握从基础 socket 到高性能异步 I/O 的完整技术栈

---

## 1. Socket 基础

### 1.1 TCP 服务器/客户端

```c
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

// TCP Echo 服务器
int tcp_server(uint16_t port) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) return -1;

    int opt = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(port),
        .sin_addr.s_addr = INADDR_ANY
    };

    if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) goto err;
    if (listen(fd, 128) < 0) goto err;

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t len = sizeof(client_addr);
        int client = accept(fd, (struct sockaddr *)&client_addr, &len);
        if (client < 0) continue;

        char buf[4096];
        ssize_t n;
        while ((n = read(client, buf, sizeof(buf))) > 0) {
            write(client, buf, n);  // echo
        }
        close(client);
    }
err:
    close(fd);
    return -1;
}
```

### 1.2 非阻塞 I/O

```c
#include <fcntl.h>

// 设置非阻塞模式
void set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

// 非阻塞 accept
int accept_nonblocking(int listen_fd) {
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);
    int fd = accept4(listen_fd, (struct sockaddr *)&addr, &len, SOCK_NONBLOCK);
    // 或使用 accept + fcntl
    return fd;
}
```

---

## 2. epoll：Linux 高性能 I/O 多路复用

### 2.1 基本 epoll 服务器

```c
#include <sys/epoll.h>

#define MAX_EVENTS 1024

int epoll_server(uint16_t port) {
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    // ... bind, listen ...
    set_nonblocking(listen_fd);

    int epoll_fd = epoll_create1(EPOLL_CLOEXEC);

    struct epoll_event ev = {
        .events = EPOLLIN,
        .data.fd = listen_fd
    };
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_fd, &ev);

    struct epoll_event events[MAX_EVENTS];

    while (1) {
        int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);

        for (int i = 0; i < nfds; i++) {
            int fd = events[i].data.fd;
            uint32_t revents = events[i].events;

            if (revents & EPOLLERR || revents & EPOLLHUP) {
                close(fd);
                continue;
            }

            if (fd == listen_fd) {
                // 新连接
                int client = accept_nonblocking(listen_fd);
                if (client >= 0) {
                    set_nonblocking(client);
                    ev.events = EPOLLIN | EPOLLET;  // 边缘触发
                    ev.data.fd = client;
                    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client, &ev);
                }
            } else if (revents & EPOLLIN) {
                // 可读
                char buf[4096];
                ssize_t n = read(fd, buf, sizeof(buf));
                if (n > 0) {
                    // 处理数据，注册可写事件
                    ev.events = EPOLLOUT | EPOLLET;
                    ev.data.fd = fd;
                    epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd, &ev);
                } else {
                    close(fd);
                }
            } else if (revents & EPOLLOUT) {
                // 可写
                // write(fd, ...)
                // 写完后改回只读
                ev.events = EPOLLIN | EPOLLET;
                ev.data.fd = fd;
                epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd, &ev);
            }
        }
    }
}
```

### 2.2 水平触发 (LT) vs 边缘触发 (ET)

| 模式 | 触发条件 | 特点 |
|:-----|:---------|:-----|
| **LT (默认)** | 只要 fd 就绪就触发 | 安全，不易丢事件 |
| **ET** | 状态变化时只触发一次 | 高效，必须一次性读完/写完 |

```c
// ET 模式必须非阻塞 + 循环读写
if (revents & EPOLLIN) {
    while (1) {
        ssize_t n = read(fd, buf, sizeof(buf));
        if (n > 0) {
            process(buf, n);
        } else if (n < 0 && errno == EAGAIN) {
            break;  // 读完了
        } else {
            close(fd);  // 错误或EOF
            break;
        }
    }
}
```

---

## 3. io_uring：下一代异步 I/O

io_uring（Linux 5.1+）是 Linux 内核提供的统一异步 I/O 接口，性能超越 epoll + 线程池。

### 3.1 基本用法

```c
#include <liburing.h>

#define ENTRIES 4096

struct io_uring ring;

void setup_uring(void) {
    io_uring_queue_init(ENTRIES, &ring, 0);
}

// 提交读请求
void submit_read(int fd, void *buf, size_t len, off_t offset) {
    struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
    io_uring_prep_read(sqe, fd, buf, len, offset);
    io_uring_sqe_set_data(sqe, (void *)(uintptr_t)fd);
    io_uring_submit(&ring);
}

// 事件循环
void event_loop(void) {
    struct io_uring_cqe *cqe;

    while (1) {
        int ret = io_uring_wait_cqe(&ring, &cqe);
        if (ret < 0) continue;

        // 处理完成事件
        int fd = (int)(uintptr_t)io_uring_cqe_get_data(cqe);
        int res = cqe->res;  // 结果（>0成功，<0错误码）

        if (res < 0) {
            // 错误处理
        } else {
            // 成功：res 为传输字节数
        }

        io_uring_cqe_seen(&ring, cqe);
    }
}
```

### 3.2 io_uring vs epoll 性能对比

| 特性 | epoll | io_uring |
|:-----|:------|:---------|
| 系统调用次数 | 每次 I/O 需 `read`/`write` | 批量提交，批量收割 |
| 缓冲区管理 | 用户态预分配 | 支持 registered buffers |
| 文件 I/O | 同步阻塞或线程池 | 原生异步 |
| 网络 I/O | 事件通知 + 用户读写 | 内核直接完成读写 |
| 零拷贝 | `sendfile` | `splice` via uring |

---

## 4. 零拷贝技术

### 4.1 sendfile（磁盘到网络）

```c
#include <sys/sendfile.h>

// 将文件内容直接发送到 socket（不经过用户态缓冲区）
void send_file(int sockfd, const char *path) {
    int filefd = open(path, O_RDONLY);
    off_t offset = 0;
    struct stat st;
    fstat(filefd, &st);

    sendfile(sockfd, filefd, &offset, st.st_size);
    close(filefd);
}
```

### 4.2 splice（管道到管道/管道到socket）

```c
#include <fcntl.h>

// 在管道和 socket 之间直接传输数据
int pipefd[2];
pipe(pipefd);

// 从文件读取到管道
splice(filefd, NULL, pipefd[1], NULL, len, SPLICE_F_MOVE);

// 从管道发送到 socket
splice(pipefd[0], NULL, sockfd, NULL, len, SPLICE_F_MOVE);
```

---

## 5. 高性能服务器设计模式

### 5.1 Reactor 模式

```
┌─────────────────────────────────────────────┐
│                  Reactor                      │
│  ┌─────────┐  ┌─────────┐  ┌─────────┐     │
│  │ Accept  │  │  Read   │  │  Write  │     │
│  │ Handler │  │ Handler │  │ Handler │     │
│  └────┬────┘  └────┬────┘  └────┬────┘     │
│       └─────────────┴─────────────┘         │
│              epoll / io_uring               │
└─────────────────────────────────────────────┘
           单线程事件循环 + 非阻塞 I/O
```

### 5.2 Proactor 模式（io_uring）

```
┌─────────────────────────────────────────────┐
│                  Proactor                     │
│                                             │
│  1. 提交异步读请求 → 内核完成读 → 回调处理   │
│  2. 提交异步写请求 → 内核完成写 → 回调处理   │
│                                             │
│  用户态无需读写，只处理完成的 buffer          │
└─────────────────────────────────────────────┘
```

---

## 6. 检查清单

- [ ] 始终检查 `read`/`write`/`accept` 的返回值和 `errno`
- [ ] 使用 `SO_REUSEADDR` 避免 TIME_WAIT 导致绑定失败
- [ ] 边缘触发模式必须非阻塞 + 循环读写
- [ ] 大文件传输使用 `sendfile` 零拷贝
- [ ] 10K+ 连接使用 epoll，100K+ 考虑 io_uring
- [ ] 处理 `EAGAIN`/`EWOULDBLOCK` 和 `EINTR`

---

## 7. 配套代码示例

本节配套可编译代码位于 `examples/network/` 目录：

| 示例文件 | 演示内容 | 编译命令 |
|---------|---------|---------|
| `tcp_server.c` | 单线程 `select` 多路复用 Echo 服务器 | `gcc -O2 -std=c11` |
| `tcp_client.c` | TCP 客户端，`gethostbyname` + `connect` | `gcc -O2 -std=c11` |

系统编程目录还有 `epoll_server.c`（Linux 专用）：

| 示例文件 | 演示内容 | 编译命令 |
|---------|---------|---------|
| `../system/epoll_server.c` | `epoll` 边缘触发多路复用服务器 | `gcc -O2 -std=c11` |

### I/O 模型性能对比

| 模型 | 单进程连接数 | CPU 开销 | 实现复杂度 | 适用场景 |
|------|------------|---------|-----------|---------|
| 阻塞 + 多进程 | 低（进程数限制） | 高（上下文切换） | 低 | 低并发 |
| `select`/`poll` | ~1000 | 中（每次遍历所有 fd） | 低 | 中等并发 |
| `epoll` (LT/ET) | 100K+ | 低（事件驱动） | 中 | 高并发服务器 |
| `io_uring` | 1M+ | 极低（零拷贝 + 批处理） | 高 | 极致性能 |

---

## 8. 常见错误模式

| 错误模式 | 后果 | 修复方案 |
|---------|------|---------|
| **未检查 `read`/`write` 返回值** | 数据截断或连接异常未处理 | 总是检查返回值，处理 `<= 0` |
| **缓冲区溢出** | 栈/堆溢出，安全漏洞 | 使用 `snprintf`，限制读取大小 |
| **未处理 `EINTR`** | 信号中断导致操作未完成 | 用 `TEMP_FAILURE_RETRY` 宏包裹 |
| **忘记 `SO_REUSEADDR`** | 服务器重启后绑定失败 | `setsockopt(fd, SO_REUSEADDR, ...)` |
| **边缘触发未循环读写** | 数据残留，下次事件丢失 | ET 模式下必须 `while(read > 0)` |
| **未设置非阻塞** | 单线程服务器被单个连接阻塞 | `fcntl(fd, F_SETFL, O_NONBLOCK)` |
| **文件描述符泄漏** | 连接关闭后未 `close(fd)` | 每个 `accept` 对应一个 `close` |

---

> **最后更新**: 2026-05-11
> **参考**: Unix Network Programming (Stevens), Linux Kernel io_uring docs
