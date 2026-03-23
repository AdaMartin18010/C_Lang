# RDMA 网络编程

## 概述

RDMA（Remote Direct Memory Access，远程直接内存访问）是一种高性能网络技术，允许网络适配器直接访问应用程序内存，无需操作系统内核介入，从而实现超低延迟和高吞吐量的数据传输。

## 核心优势

### 1. 零拷贝（Zero Copy）

传统网络通信需要多次数据拷贝：

- 网卡 → 内核缓冲区 → 用户缓冲区
- RDMA 消除了这些拷贝，数据直接从网卡写入目标内存

### 2. 内核旁路（Kernel Bypass）

- 应用程序直接与网卡通信
- 避免了系统调用的上下文切换开销
- 延迟可降至微秒级别

### 3. CPU 卸载（CPU Offload）

- 网络协议处理由网卡硬件完成
- 释放 CPU 用于计算任务
- 适合高并发场景

## RDMA 实现方式

### InfiniBand (IB)

InfiniBand 是专为 RDMA 设计的高速网络架构：

```c
// InfiniBand 设备初始化示例
#include <infiniband/verbs.h>

struct ibv_device **dev_list;
struct ibv_device *ib_dev;
struct ibv_context *ctx;
int num_devices;

// 获取可用设备列表
dev_list = ibv_get_device_list(&num_devices);
if (!dev_list) {
    perror("Failed to get IB devices list");
    return -1;
}

// 选择第一个设备
ib_dev = dev_list[0];
if (!ib_dev) {
    fprintf(stderr, "No IB devices found\n");
    return -1;
}

// 打开设备上下文
ctx = ibv_open_device(ib_dev);
if (!ctx) {
    fprintf(stderr, "Failed to open device %s\n",
            ibv_get_device_name(ib_dev));
    return -1;
}

printf("Opened device: %s\n", ibv_get_device_name(ib_dev));
```

### RoCE (RDMA over Converged Ethernet)

RoCE 允许在标准以太网基础设施上运行 RDMA：

```c
// RoCE 配置示例
#include <rdma/rdma_cma.h>

struct rdma_event_channel *ec;
struct rdma_cm_id *conn_id;
struct ibv_qp_init_attr qp_attr = {};

// 创建事件通道
ec = rdma_create_event_channel();
if (!ec) {
    perror("rdma_create_event_channel");
    return -1;
}

// 创建 RDMA 连接标识符
ret = rdma_create_id(ec, &conn_id, NULL, RDMA_PS_TCP);
if (ret) {
    perror("rdma_create_id");
    return -1;
}

// 解析地址
struct sockaddr_in addr = {};
addr.sin_family = AF_INET;
addr.sin_port = htons(port);
inet_pton(AF_INET, server_ip, &addr.sin_addr);

ret = rdma_resolve_addr(conn_id, NULL,
                        (struct sockaddr *)&addr, 2000);
if (ret) {
    perror("rdma_resolve_addr");
    return -1;
}
```

## Verbs API 详解

Verbs API 是 RDMA 编程的核心接口，提供底层硬件访问能力。

### 保护域（Protection Domain）

```c
// 创建保护域
struct ibv_pd *pd;
pd = ibv_alloc_pd(ctx);
if (!pd) {
    fprintf(stderr, "Failed to allocate PD\n");
    return -1;
}

// 保护域用于管理内存区域和队列对的访问权限
```

### 内存区域（Memory Region）

```c
// 注册内存区域
void *buf;
size_t buf_size = 1024 * 1024; // 1MB
struct ibv_mr *mr;

// 分配对齐内存
buf = posix_memalign(&buf, 4096, buf_size);
if (!buf) {
    perror("posix_memalign");
    return -1;
}

// 注册内存区域
mr = ibv_reg_mr(pd, buf, buf_size,
                IBV_ACCESS_LOCAL_WRITE |
                IBV_ACCESS_REMOTE_WRITE |
                IBV_ACCESS_REMOTE_READ);
if (!mr) {
    fprintf(stderr, "Failed to register MR\n");
    return -1;
}

printf("MR registered: lkey=0x%x, rkey=0x%x\n",
       mr->lkey, mr->rkey);
```

### 完成队列（Completion Queue）

```c
// 创建完成队列
struct ibv_cq *cq;
cq = ibv_create_cq(ctx, 100, NULL, NULL, 0);
if (!cq) {
    fprintf(stderr, "Failed to create CQ\n");
    return -1;
}

// 请求完成通知
ibv_req_notify_cq(cq, 0);
```

### 队列对（Queue Pair）

```c
// 创建队列对
struct ibv_qp *qp;
struct ibv_qp_init_attr qp_attr = {};

qp_attr.send_cq = cq;
qp_attr.recv_cq = cq;
qp_attr.qp_type = IBV_QPT_RC; // Reliable Connection
qp_attr.sq_sig_all = 1;
qp_attr.cap.max_send_wr = 100;
qp_attr.cap.max_recv_wr = 100;
qp_attr.cap.max_send_sge = 1;
qp_attr.cap.max_recv_sge = 1;

qp = ibv_create_qp(pd, &qp_attr);
if (!qp) {
    fprintf(stderr, "Failed to create QP\n");
    return -1;
}
```

### RDMA 操作类型

#### 1. Send/Recv（双边操作）

```c
// 发送操作
struct ibv_sge sg;
struct ibv_send_wr wr = {};
struct ibv_send_wr *bad_wr;

sg.addr = (uint64_t)send_buf;
sg.length = data_len;
sg.lkey = mr->lkey;

wr.wr_id = 1;
wr.opcode = IBV_WR_SEND;
wr.sg_list = &sg;
wr.num_sge = 1;
wr.send_flags = IBV_SEND_SIGNALED;

ret = ibv_post_send(qp, &wr, &bad_wr);
if (ret) {
    fprintf(stderr, "Failed to post send\n");
    return -1;
}

// 接收操作
struct ibv_recv_wr recv_wr = {};
struct ibv_recv_wr *bad_recv_wr;
struct ibv_sge recv_sg;

recv_sg.addr = (uint64_t)recv_buf;
recv_sg.length = buf_size;
recv_sg.lkey = recv_mr->lkey;

recv_wr.wr_id = 2;
recv_wr.sg_list = &recv_sg;
recv_wr.num_sge = 1;

ret = ibv_post_recv(qp, &recv_wr, &bad_recv_wr);
if (ret) {
    fprintf(stderr, "Failed to post recv\n");
    return -1;
}
```

#### 2. RDMA Write（单边写）

```c
// RDMA 写操作 - 直接写入远程内存
struct ibv_sge write_sg;
struct ibv_send_wr write_wr = {};

write_sg.addr = (uint64_t)local_buf;
write_sg.length = data_len;
write_sg.lkey = local_mr->lkey;

write_wr.wr_id = 3;
write_wr.opcode = IBV_WR_RDMA_WRITE;
write_wr.sg_list = &write_sg;
write_wr.num_sge = 1;
write_wr.wr.rdma.remote_addr = remote_addr;
write_wr.wr.rdma.rkey = remote_rkey;
write_wr.send_flags = IBV_SEND_SIGNALED;

ret = ibv_post_send(qp, &write_wr, &bad_wr);
```

#### 3. RDMA Read（单边读）

```c
// RDMA 读操作 - 直接读取远程内存
struct ibv_sge read_sg;
struct ibv_send_wr read_wr = {};

read_sg.addr = (uint64_t)local_buf;
read_sg.length = data_len;
read_sg.lkey = local_mr->lkey;

read_wr.wr_id = 4;
read_wr.opcode = IBV_WR_RDMA_READ;
read_wr.sg_list = &read_sg;
read_wr.num_sge = 1;
read_wr.wr.rdma.remote_addr = remote_addr;
read_wr.wr.rdma.rkey = remote_rkey;
read_wr.send_flags = IBV_SEND_SIGNALED;

ret = ibv_post_send(qp, &read_wr, &bad_wr);
```

#### 4. 原子操作

```c
// 原子比较并交换（Compare and Swap）
struct ibv_sge atomic_sg;
struct ibv_send_wr atomic_wr = {};
uint64_t compare = expected_value;
uint64_t swap = new_value;

atomic_sg.addr = (uint64_t)&local_result;
atomic_sg.length = sizeof(uint64_t);
atomic_sg.lkey = local_mr->lkey;

atomic_wr.wr_id = 5;
atomic_wr.opcode = IBV_WR_ATOMIC_CMP_AND_SWP;
atomic_wr.sg_list = &atomic_sg;
atomic_wr.num_sge = 1;
atomic_wr.wr.atomic.remote_addr = remote_addr;
atomic_wr.wr.atomic.rkey = remote_rkey;
atomic_wr.wr.atomic.compare_add = compare;
atomic_wr.wr.atomic.swap = swap;

ret = ibv_post_send(qp, &atomic_wr, &bad_wr);
```

## 完成事件处理

```c
// 轮询完成队列
void poll_completions(struct ibv_cq *cq, int expected) {
    struct ibv_wc wc;
    int completed = 0;

    while (completed < expected) {
        int ret = ibv_poll_cq(cq, 1, &wc);
        if (ret < 0) {
            fprintf(stderr, "Poll CQ failed\n");
            break;
        }
        if (ret == 0)
            continue;

        if (wc.status != IBV_WC_SUCCESS) {
            fprintf(stderr, "Work completion error: %s\n",
                    ibv_wc_status_str(wc.status));
            continue;
        }

        printf("Completion: wr_id=%lu, opcode=%d\n",
               wc.wr_id, wc.opcode);
        completed++;
    }
}
```

## 性能优化技巧

### 1. 批量处理

```c
// 批量发送多个 WR
struct ibv_send_wr wrs[BATCH_SIZE];
struct ibv_sge sgs[BATCH_SIZE];

for (int i = 0; i < BATCH_SIZE; i++) {
    sgs[i].addr = (uint64_t)bufs[i];
    sgs[i].length = buf_sizes[i];
    sgs[i].lkey = mrs[i]->lkey;

    wrs[i].wr_id = i;
    wrs[i].opcode = IBV_WR_SEND;
    wrs[i].sg_list = &sgs[i];
    wrs[i].num_sge = 1;
    wrs[i].send_flags = (i == BATCH_SIZE - 1) ?
                        IBV_SEND_SIGNALED : 0;
    wrs[i].next = (i < BATCH_SIZE - 1) ? &wrs[i + 1] : NULL;
}

ibv_post_send(qp, &wrs[0], &bad_wr);
```

### 2. 使用 SRQ（Shared Receive Queue）

```c
// 创建共享接收队列
struct ibv_srq_init_attr srq_attr = {};
srq_attr.attr.max_wr = 1000;
srq_attr.attr.max_sge = 1;

struct ibv_srq *srq = ibv_create_srq(pd, &srq_attr);
```

### 3. 内存池管理

```c
// 预分配内存池
#define POOL_SIZE 1024
#define CHUNK_SIZE 4096

typedef struct {
    void *buf;
    struct ibv_mr *mr;
    int in_use;
} mempool_entry;

mempool_entry mempool[POOL_SIZE];

void init_mempool(struct ibv_pd *pd) {
    for (int i = 0; i < POOL_SIZE; i++) {
        posix_memalign(&mempool[i].buf, 4096, CHUNK_SIZE);
        mempool[i].mr = ibv_reg_mr(pd, mempool[i].buf,
                                   CHUNK_SIZE,
                                   IBV_ACCESS_LOCAL_WRITE |
                                   IBV_ACCESS_REMOTE_WRITE);
        mempool[i].in_use = 0;
    }
}
```

## 调试与诊断

```c
// 查询端口信息
struct ibv_port_attr port_attr;
ret = ibv_query_port(ctx, 1, &port_attr);
if (ret) {
    fprintf(stderr, "Failed to query port\n");
    return -1;
}

printf("Port Info:\n");
printf("  State: %d\n", port_attr.state);
printf("  LID: %d\n", port_attr.lid);
printf("  MTU: %d\n", port_attr.active_mtu);
printf("  Speed: %d\n", port_attr.active_speed);
```

## 参考资源

- [RDMAmojo - RDMA Programming Guide](http://www.rdmamojo.com/)
- [Linux RDMA 内核文档](https://www.kernel.org/doc/html/latest/infiniband/)
- [Mellanox OFED 文档](https://docs.nvidia.com/networking/)


---

## 深入理解

### 核心原理

深入探讨技术原理和实现细节。

### 实践应用

- 应用场景1
- 应用场景2
- 应用场景3

### 最佳实践

1. 理解基础概念
2. 掌握核心机制
3. 应用到实际项目

---

> **最后更新**: 2026-03-21
> **维护者**: AI Code Review
