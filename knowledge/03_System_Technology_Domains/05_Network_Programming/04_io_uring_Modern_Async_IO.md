# io_uring：现代Linux异步I/O深度指南

> **标准**: Linux Kernel 5.1+ (基础), 6.10+ (send/recv bundles), 6.13+ (ring resizing, registered wait regions), 6.17+ (zero-copy RX)
> **前置知识**: [POSIX网络编程](04_DPDK_High_Performance_Networking.md), [C11原子操作](../../01_Core_Knowledge_System/14_Concurrency_Parallelism/README.md)
> **后续延伸**: [DPDK内核旁路](../35_RDMA_Network/README.md), [高性能数据库I/O](../09_Database_Internals/01_B_Tree_Implementation_Case.md)
> **对标资源**: Jens Axboe (io_uring作者), VLDB 2026, Linux Kernel文档, techbytes.app 2026
> **难度**: 5/5 | **预计学习时间**: 6-8 小时

---

## 目录

- [io_uring：现代Linux异步I/O深度指南](#)
  - [目录](#目录)

---

## 一、为什么需要io_uring

### 1.1 传统I/O接口的局限

Linux传统I/O接口存在**双重开销**：

| 接口 | 问题 | 每I/O开销 |
|:-----|:-----|:----------|
| **read/write** | 阻塞调用，每次I/O需要两次上下文切换 | ~1.5μs |
| **epoll + read/write** | 非阻塞但read/write仍是同步系统调用 | ~1.2μs |
| **aio (POSIX AIO)** | 线程池模拟，非真异步；API复杂；限制多 | ~2-5μs |
| **DPDK** | 内核旁路，需要独占网卡，运维成本高 | ~0.1μs（但配置复杂） |

### 1.2 io_uring的核心优势

io_uring（由Jens Axboe于2019年设计）是Linux的**真异步I/O接口**，通过**共享内存环形队列**实现用户态与内核态的高效协作：

```text
用户空间                          内核空间
┌─────────────────┐              ┌─────────────────┐
│  Submission Queue │  mmap        │  io_uring 核心   │
│  (SQ) — 用户写    │◄────────────►│  SQ 拷贝 + 执行  │
│  256-4096 entries │              │                 │
├─────────────────┤              │  Completion Queue │
│  Completion Queue │  mmap        │  (CQ) — 内核写   │
│  (CQ) — 用户读    │◄────────────►│  结果返回        │
│  256-4096 entries │              │                 │
├─────────────────┤              ├─────────────────┤
│  SQEs/CQEs      │              │  实际I/O执行     │
│  (共享内存)      │              │  (文件/网络/...) │
└─────────────────┘              └─────────────────┘
```

**核心优势**：

1. **零系统调用批量提交**：一次 `io_uring_enter` 提交数百个I/O
2. **零拷贝**：registered buffers避免内核-用户态数据拷贝
3. **统一接口**：文件、网络、超时、信号统一在io_uring中处理
4. **Polling模式**：内核线程轮询，完全绕过系统调用

---

## 二、架构原理

### 2.1 环形队列数据结构

```c
// 简化表示：io_uring的核心是两个无锁环形队列

struct io_uring {
    int ring_fd;          // io_uring文件描述符

    // Submission Queue (用户写，内核读)
    struct io_uring_sqe *sqes;   // SQE数组（实际提交条目）
    unsigned *sq_head;    // 内核消费位置（内核更新）
    unsigned *sq_tail;    // 用户生产位置（用户更新）
    unsigned *sq_mask;    // 数组掩码（size-1）
    unsigned *sq_flags;   // 标志位（如IORING_SQ_NEED_WAKEUP）

    // Completion Queue (内核写，用户读)
    struct io_uring_cqe *cqes;   // CQE数组（完成条目）
    unsigned *cq_head;    // 用户消费位置（用户更新）
    unsigned *cq_tail;    // 内核生产位置（内核更新）
    unsigned *cq_mask;
};
```

### 2.2 SQE（Submission Queue Entry）

```c
struct io_uring_sqe {
    __u8  opcode;         // 操作码：read, write, accept, recv, send...
    __u8  flags;          // 标志：IOSQE_FIXED_FILE, IOSQE_IO_LINK...
    __u16 ioprio;         // I/O优先级
    __s32 fd;             // 文件描述符
    union {
        __u64 off;        // 文件偏移
        __u64 addr2;      // 额外地址
    };
    __u64 addr;           // 用户缓冲区地址
    __u32 len;            // 长度
    union {
        __kernel_rwf_t rw_flags;  // read/write标志
        __u32 accept_flags;       // accept标志
        __u32 cancel_flags;       // cancel标志
    };
    __u64 user_data;      // 用户数据（不透明，内核原样返回）
    union {
        __u16 buf_index;  // 注册缓冲区索引
        __u64 __pad2[3];
    };
};
```

### 2.3 CQE（Completion Queue Entry）

```c
struct io_uring_cqe {
    __u64 user_data;      // 对应SQE的user_data
    __s32 res;            // 结果：成功时=字节数，失败时=-errno
    __u32 flags;          // 标志位
};
```

> **关键设计**：`user_data` 是io_uring的核心关联机制。用户将请求上下文（如指针、ID）放入SQE的`user_data`，内核在CQE中原样返回，无需额外查找表。

---

## 三、基础API与核心概念

### 3.1 初始化与销毁

```c
#include <liburing.h>

// 初始化io_uring实例
struct io_uring ring;
unsigned entries = 4096;  // 队列深度（推荐4096 for high throughput）
unsigned flags = 0;

int ret = io_uring_queue_init(entries, &ring, flags);
if (ret < 0) {
    fprintf(stderr, "io_uring init failed: %s\n", strerror(-ret));
    return -1;
}

// 使用完毕
io_uring_queue_exit(&ring);
```

**常用初始化标志**：

| 标志 | 说明 | 适用场景 |
|:-----|:-----|:---------|
| `IORING_SETUP_IOPOLL` | 轮询模式（需O_DIRECT） | NVMe SSD，极低延迟 |
| `IORING_SETUP_SQPOLL` | 内核线程轮询SQ | 高吞吐，减少syscall |
| `IORING_SETUP_SQ_AFF` | 绑定SQ线程到特定CPU | 减少CPU迁移 |
| `IORING_SETUP_COOP_TASKRUN` | 协作式任务调度 | 减少IPI中断 |
| `IORING_SETUP_SINGLE_ISSUER` | 单生产者优化 | 单线程提交 |
| `IORING_SETUP_DEFER_TASKRUN` | 延迟任务执行到enter调用 | 高吞吐数据库（推荐） |

### 3.2 推荐的DBMS配置（Linux 6.10+）

```c
// VLDB 2026 推荐的io_uring配置
struct io_uring_params params = {0};
params.flags = IORING_SETUP_SINGLE_ISSUER | IORING_SETUP_DEFER_TASKRUN;

int ret = io_uring_queue_init_params(4096, &ring, &params);
if (ret < 0) {
    fprintf(stderr, "io_uring init failed: %s\n", strerror(-ret));
    return -1;
}
```

> **说明**: `SINGLE_ISSUER` + `DEFER_TASKRUN` 是数据库工作负载的**黄金组合**。DEFER_TASKRUN避免内核IPI抢占应用线程，SINGLE_ISSUER启用单生产者优化。

### 3.3 提交I/O请求

```c
// 获取一个SQE
struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
if (!sqe) {
    // SQ已满，需要先提交
    io_uring_submit(&ring);
    sqe = io_uring_get_sqe(&ring);
}

// 准备读操作
int fd = open("data.bin", O_RDONLY | O_DIRECT);  // O_DIRECT推荐用于io_uring
char buf[4096] __attribute__((aligned(4096)));
io_uring_prep_read(sqe, fd, buf, 4096, 0);  // offset=0

// 设置用户数据（请求上下文）
io_uring_sqe_set_data(sqe, (void*)(uintptr_t)fd);

// 提交到内核
int submitted = io_uring_submit(&ring);
if (submitted < 0) {
    fprintf(stderr, "submit failed: %s\n", strerror(-submitted));
}
```

### 3.4 收割完成事件

```c
// 等待至少1个完成事件
struct io_uring_cqe *cqe;
int ret = io_uring_wait_cqe(&ring, &cqe);
if (ret < 0) {
    fprintf(stderr, "wait_cqe failed: %s\n", strerror(-ret));
    return -1;
}

// 处理完成事件
if (cqe->res < 0) {
    // I/O失败
    fprintf(stderr, "I/O error: %s\n", strerror(-cqe->res));
} else {
    // I/O成功，res = 读取/写入的字节数
    printf("I/O completed: %d bytes\n", cqe->res);

    // 获取用户数据
    int fd = (int)(uintptr_t)io_uring_cqe_get_data(cqe);
    close(fd);
}

// 标记为已消费
io_uring_cqe_seen(&ring, cqe);
```

### 3.5 批量提交与收割

```c
// 批量准备多个SQE
for (int i = 0; i < batch_size; i++) {
    struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
    io_uring_prep_read(sqe, fds[i], bufs[i], sizes[i], offsets[i]);
    io_uring_sqe_set_data(sqe, (void*)(uintptr_t)i);
}

// 一次性提交整个批次
io_uring_submit(&ring);

// 批量收割
struct io_uring_cqe *cqes[batch_size];
int completed = io_uring_peek_batch_cqe(&ring, cqes, batch_size);
for (int i = 0; i < completed; i++) {
    int idx = (int)(uintptr_t)io_uring_cqe_get_data(cqes[i]);
    if (cqes[i]->res < 0) {
        fprintf(stderr, "Request %d failed: %s\n", idx, strerror(-cqes[i]->res));
    }
    io_uring_cqe_seen(&ring, cqes[i]);
}
```

---

## 四、存储I/O实战

### 4.1 异步文件读取

```c
#define _GNU_SOURCE
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <liburing.h>

#define QUEUE_DEPTH 4096
#define BLOCK_SIZE 4096

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        return 1;
    }

    struct io_uring ring;
    int ret = io_uring_queue_init(QUEUE_DEPTH, &ring, 0);
    if (ret < 0) {
        perror("io_uring_queue_init");
        return 1;
    }

    int fd = open(argv[1], O_RDONLY | O_DIRECT);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    // 分配对齐内存（O_DIRECT要求）
    char *buf;
    ret = posix_memalign((void **)&buf, BLOCK_SIZE, BLOCK_SIZE);
    if (ret != 0) {
        fprintf(stderr, "posix_memalign failed\n");
        return 1;
    }

    // 提交异步读请求
    struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
    io_uring_prep_read(sqe, fd, buf, BLOCK_SIZE, 0);
    io_uring_sqe_set_data(sqe, buf);
    io_uring_submit(&ring);

    // 等待完成
    struct io_uring_cqe *cqe;
    ret = io_uring_wait_cqe(&ring, &cqe);
    if (ret < 0) {
        fprintf(stderr, "wait_cqe: %s\n", strerror(-ret));
        return 1;
    }

    if (cqe->res < 0) {
        fprintf(stderr, "Read error: %s\n", strerror(-cqe->res));
    } else {
        printf("Read %d bytes\n", cqe->res);
    }

    io_uring_cqe_seen(&ring, cqe);
    free(buf);
    close(fd);
    io_uring_queue_exit(&ring);
    return 0;
}
```

**编译**：

```bash
gcc -O2 -o io_uring_read io_uring_read.c -luring
```

### 4.2 批量异步写（数据库WAL模拟）

```c
// 模拟数据库批量写入WAL日志
#define BATCH_SIZE 64
#define ENTRY_SIZE 4096

int batch_write_wal(struct io_uring *ring, int fd,
                    char **entries, size_t count) {
    size_t submitted = 0;
    off_t offset = lseek(fd, 0, SEEK_END);

    while (submitted < count) {
        size_t batch = (count - submitted < BATCH_SIZE)
                       ? (count - submitted) : BATCH_SIZE;

        for (size_t i = 0; i < batch; i++) {
            struct io_uring_sqe *sqe = io_uring_get_sqe(ring);
            io_uring_prep_write(sqe, fd, entries[submitted + i],
                               ENTRY_SIZE, offset + (submitted + i) * ENTRY_SIZE);
            io_uring_sqe_set_data(sqe, (void *)(submitted + i));
        }

        int ret = io_uring_submit(ring);
        if (ret < 0) return ret;
        submitted += batch;
    }

    // 等待所有完成
    for (size_t i = 0; i < count; i++) {
        struct io_uring_cqe *cqe;
        io_uring_wait_cqe(ring, &cqe);
        if (cqe->res < 0) {
            fprintf(stderr, "Write %zu failed: %s\n",
                    (size_t)cqe->user_data, strerror(-cqe->res));
        }
        io_uring_cqe_seen(ring, cqe);
    }

    return 0;
}
```

---

## 五、网络I/O实战

### 5.1 io_uring TCP Echo服务器

```c
#include <arpa/inet.h>
#include <liburing.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8080
#define BACKLOG 128
#define BUFFER_SIZE 4096
#define MAX_CONNECTIONS 1024

typedef enum {
    ACCEPT,
    READ,
    WRITE,
    CLOSE
} conn_state_t;

typedef struct {
    int fd;
    conn_state_t state;
    char buf[BUFFER_SIZE];
    size_t buf_len;
} conn_t;

static conn_t connections[MAX_CONNECTIONS];

void add_accept(struct io_uring *ring, int listen_fd) {
    struct io_uring_sqe *sqe = io_uring_get_sqe(ring);
    io_uring_prep_accept(sqe, listen_fd, NULL, NULL, 0);
    io_uring_sqe_set_data(sqe, (void *)(uintptr_t)-1);  // -1 = accept op
}

void add_read(struct io_uring *ring, int idx) {
    struct io_uring_sqe *sqe = io_uring_get_sqe(ring);
    io_uring_prep_recv(sqe, connections[idx].fd,
                       connections[idx].buf, BUFFER_SIZE, 0);
    io_uring_sqe_set_data(sqe, (void *)(uintptr_t)idx);
    connections[idx].state = READ;
}

void add_write(struct io_uring *ring, int idx) {
    struct io_uring_sqe *sqe = io_uring_get_sqe(ring);
    io_uring_prep_send(sqe, connections[idx].fd,
                       connections[idx].buf, connections[idx].buf_len, 0);
    io_uring_sqe_set_data(sqe, (void *)(uintptr_t)idx);
    connections[idx].state = WRITE;
}

int main() {
    struct io_uring ring;
    io_uring_queue_init(4096, &ring,
                        IORING_SETUP_SINGLE_ISSUER | IORING_SETUP_DEFER_TASKRUN);

    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    bind(listen_fd, (struct sockaddr *)&addr, sizeof(addr));
    listen(listen_fd, BACKLOG);
    printf("io_uring echo server on port %d\n", PORT);

    // 初始accept请求
    add_accept(&ring, listen_fd);
    io_uring_submit(&ring);

    while (1) {
        struct io_uring_cqe *cqe;
        int ret = io_uring_wait_cqe(&ring, &cqe);
        if (ret < 0) continue;

        int idx = (int)(uintptr_t)io_uring_cqe_get_data(cqe);

        if (idx == -1) {
            // Accept完成
            if (cqe->res >= 0) {
                int new_fd = cqe->res;
                // 查找空闲连接槽
                int new_idx = -1;
                for (int i = 0; i < MAX_CONNECTIONS; i++) {
                    if (connections[i].fd == 0) { new_idx = i; break; }
                }
                if (new_idx >= 0) {
                    connections[new_idx].fd = new_fd;
                    add_read(&ring, new_idx);
                } else {
                    close(new_fd);
                }
            }
            // 重新提交accept
            add_accept(&ring, listen_fd);
        } else {
            // 读写操作完成
            if (cqe->res <= 0) {
                // 连接关闭或错误
                close(connections[idx].fd);
                connections[idx].fd = 0;
            } else if (connections[idx].state == READ) {
                connections[idx].buf_len = cqe->res;
                add_write(&ring, idx);
            } else if (connections[idx].state == WRITE) {
                add_read(&ring, idx);
            }
        }

        io_uring_cqe_seen(&ring, cqe);
        io_uring_submit(&ring);
    }

    io_uring_queue_exit(&ring);
    return 0;
}
```

---

## 六、高级特性（Linux 6.10-6.17）

### 6.1 Send/Recv Bundles（Linux 6.10）

```c
// Linux 6.10+ 支持 send/recv bundles
// 单次接收可使用多个提供的缓冲区，减少网络栈遍历

struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);

// 使用 IORING_RECV_MULTISHOT 标志启用 multi-shot receive
io_uring_prep_recv(sqe, sock_fd, NULL, 0, 0);
sqe->ioprio = IORING_RECV_MULTISHOT;

// 配合 IORING_OP_PROVIDE_BUFFERS 提供缓冲区池
// 当数据到达时，内核自动选择一个空闲缓冲区写入
```

### 6.2 Ring Resizing（Linux 6.13+）

```c
// 动态调整队列大小，无需销毁重建
// 适用于连接数动态变化的场景

#include <linux/io_uring.h>

// 使用 IORING_SETUP_NO_SQARRAY 和 ring resize API
// 详情参考 Linux 6.13+ 的 io_uring 文档
```

### 6.3 Zero-Copy RX（Linux 6.17+）

```c
// Linux 6.17+ 的 zero-copy receive
// 移除内核到用户态的数据拷贝，同时保留TCP栈处理

// 设置 sockopt 启用 zero-copy
int val = 1;
setsockopt(sock_fd, SOL_SOCKET, SO_ZEROCOPY, &val, sizeof(val));

// io_uring 零拷贝接收
struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
io_uring_prep_recv(sqe, sock_fd, buf, len, MSG_ZEROCOPY);
```

**性能阈值**：zero-copy send 的 crossover point 约为 **3000字节** 数据包。超过此阈值时，io_uring zero-copy 优于同步 syscall。

### 6.4 Registered Buffers（零拷贝基础）

```c
// 注册固定缓冲区，避免每次I/O的内存锁定开销
#define BUF_COUNT 64
#define BUF_SIZE (1024 * 1024)  // 1MB per buffer

char *buffers[BUF_COUNT];
for (int i = 0; i < BUF_COUNT; i++) {
    posix_memalign((void **)&buffers[i], 4096, BUF_SIZE);
}

// 向io_uring注册缓冲区
ret = io_uring_register_buffers(&ring, (const struct iovec *)buffers, BUF_COUNT);
if (ret < 0) {
    fprintf(stderr, "register_buffers failed: %s\n", strerror(-ret));
    return -1;
}

// 使用注册缓冲区进行I/O
struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
io_uring_prep_read_fixed(sqe, fd, buffers[0], BUF_SIZE, 0, 0);
// 最后一个参数 0 是 buf_index
```

> **Linux 6.13+ 优化**：克隆已有的900GB缓冲区注册，设置时间从约1秒降至约17微秒。这使得动态线程/ring创建在架构上可行。

---

## 七、性能优化与调优

### 7.1 数据库负载优化指南（VLDB 2026）

基于 VLDB 2026 论文 *"io_uring for High-Performance DBMSs"*：

| 优化项 | 配置 | 效果 |
|:-------|:-----|:-----|
| **队列深度** | 4096+ | 提高设备并行度 |
| **批量大小** | 自适应（根据outstanding I/Os调整） | 降低syscall开销 |
| **ring-per-thread** | 每个工作线程一个io_uring实例 | 消除跨线程同步 |
| **SINGLE_ISSUER + DEFER_TASKRUN** | 启用 | 减少IPI，提高缓存局部性 |
| **Registered Buffers** | 4KB对齐，大容量 | 消除内存锁定开销 |
| **O_DIRECT** | 存储I/O | 绕过页缓存，减少拷贝 |
| **Polling模式** | 低延迟NVMe | 完全绕过系统调用 |

### 7.2 自适应批处理算法

```c
// 根据当前in-flight I/O数量和等待fiber数量动态调整批处理大小
// 这是VLDB 2026中buffer manager的关键优化

typedef struct {
    size_t batch_size;
    size_t max_batch;
    size_t min_batch;
    double target_ratio;  // outstanding_ios / waiting_fibers
} adaptive_batch_t;

size_t compute_batch_size(adaptive_batch_t *cfg,
                          size_t outstanding_ios,
                          size_t waiting_fibers) {
    if (waiting_fibers == 0) return cfg->min_batch;

    double ratio = (double)outstanding_ios / waiting_fibers;
    if (ratio > cfg->target_ratio) {
        // I/O太多，增加批处理以提高amortization
        cfg->batch_size = (cfg->batch_size + cfg->max_batch) / 2;
    } else {
        // I/O太少，减少批处理以减少延迟
        cfg->batch_size = (cfg->batch_size + cfg->min_batch) / 2;
    }
    return cfg->batch_size;
}
```

---

## 八、常见陷阱与调试

### 8.1 所有权陷阱：缓冲区生命周期

```c
// ❌ 错误：缓冲区在I/O完成前被释放
void bad_example(struct io_uring *ring) {
    char buf[1024];  // 栈缓冲区
    struct io_uring_sqe *sqe = io_uring_get_sqe(ring);
    io_uring_prep_read(sqe, fd, buf, 1024, 0);
    io_uring_submit(ring);
    // 函数返回，buf被释放，但I/O可能还未完成！
}

// ✅ 正确：缓冲区必须在I/O完成后才能释放
void good_example(struct io_uring *ring) {
    char *buf = malloc(1024);
    struct io_uring_sqe *sqe = io_uring_get_sqe(ring);
    io_uring_prep_read(sqe, fd, buf, 1024, 0);
    io_uring_sqe_set_data(sqe, buf);  // 将buf指针作为user_data
    io_uring_submit(ring);
}

// 在CQE处理中释放
void handle_completion(struct io_uring_cqe *cqe) {
    char *buf = (char *)io_uring_cqe_get_data(cqe);
    // 使用buf...
    free(buf);  // 安全：I/O已完成
}
```

### 8.2 并发陷阱：多线程SQ访问

```c
// ❌ 错误：多线程同时操作同一个io_uring的SQ
// io_uring的SQ不是线程安全的（除非使用IORING_SETUP_SQPOLL）

// ✅ 正确方案1：每个线程独立的io_uring实例
// ✅ 正确方案2：使用IORING_SETUP_SQPOLL + 单生产者
// ✅ 正确方案3：外部锁保护SQ操作
```

### 8.3 功能探测

```c
// 运行时探测io_uring特性支持
#include <linux/io_uring.h>

bool probe_feature(struct io_uring *ring, int op) {
    struct io_uring_probe *probe = io_uring_get_probe_ring(ring);
    if (!probe) return false;

    bool supported = io_uring_opcode_supported(probe, op);
    io_uring_free_probe(probe);
    return supported;
}

// 使用示例
if (probe_feature(&ring, IORING_OP_SEND_ZC)) {
    printf("Zero-copy send supported\n");
}
```

### 8.4 调试工具

| 工具 | 用途 |
|:-----|:-----|
| `strace -e io_uring_enter,io_uring_register` | 跟踪io_uring系统调用 |
| `perf trace -e io_uring:*` | 使用perf跟踪io_uring事件 |
| `/sys/kernel/debug/tracing/events/io_uring/` | ftrace事件跟踪 |
| `io_uring-cp` (liburing examples) | 基准测试工具 |

---

## 九、与epoll/select的对比

| 特性 | select/poll | epoll | io_uring |
|:-----|:------------|:------|:---------|
| **接口类型** | 同步通知 | 同步通知 | 真异步 |
| **系统调用/事件** | O(n) | O(1) | 0（polling模式） |
| **批量处理** | ❌ 不支持 | ❌ 不支持 | ✅ 支持 |
| **文件I/O** | ❌ 不支持 | ❌ 不支持 | ✅ 支持 |
| **网络I/O** | ✅ 支持 | ✅ 支持 | ✅ 支持 |
| **零拷贝** | ❌ 不支持 | ❌ 不支持 | ✅ 支持 |
| **Buffer注册** | ❌ 不支持 | ❌ 不支持 | ✅ 支持 |
| **Polling模式** | ❌ 不支持 | ❌ 不支持 | ✅ 支持 |
| **复杂度** | 低 | 中 | 高 |
| **适用场景** | <100 fd | <10K fd, 连接型 | 高吞吐DB/存储/网络 |

---

## 十、参考资源

### 10.1 官方资源

| 资源 | URL | 说明 |
|:-----|:----|:-----|
| Linux Kernel io_uring文档 | `Documentation/io_uring/` in kernel tree | 权威参考 |
| liburing GitHub | <https://github.com/axboe/liburing> | 用户态库 |
| Jens Axboe的博客 | <https://kernel.dk> | io_uring作者 |
| io_uring man pages | `man io_uring_setup`, `man io_uring_enter` | API参考 |

### 10.2 学术论文

| 论文 | 会议 | 年份 | 核心贡献 |
|:-----|:-----|:----:|:---------|
| *io_uring for High-Performance DBMSs* | VLDB | 2026 | 数据库集成指南，自适应批处理 |
| *Asynchronous I/O APIs Overview* | emergentmind.com | 2026 | 2026年异步I/O全景 |
| *Linux io_uring in 2026* | techbytes.app | 2026 | 新特性详解（6.10-6.17） |

### 10.3 性能基准

| 场景 | io_uring vs 同步 | vs epoll | 来源 |
|:-----|:----------------|:---------|:-----|
| NVMe seq read | 5-6x faster | N/A | Jens Axboe, 6.11/6.12 |
| 6-node shuffle (400Gbps) | N/A | 优于epoll+blocking | VLDB 2026 |
| PostgreSQL buffer manager | 14% improvement | N/A | VLDB 2026 |
| 900GB buffer reg clone | 1s → 17μs | N/A | Linux 6.13+ |

---

> **总结**: io_uring是Linux I/O的**范式转变**，通过共享内存环形队列实现了用户态与内核态的**零拷贝、零系统调用协作**。在Linux 6.10-6.17中，send/recv bundles、ring resizing、zero-copy RX等新特性进一步扩展了其适用场景。对于高吞吐数据库、网络服务器和存储系统，io_uring已取代epoll成为**事实标准**。掌握io_uring是2025-2026年系统C程序员的**必备技能**。
