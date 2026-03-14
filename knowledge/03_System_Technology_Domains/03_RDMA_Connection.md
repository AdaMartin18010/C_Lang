# RDMA连接管理

## 目录

- [RDMA连接管理](#rdma连接管理)
  - [目录](#目录)
  - [RDMA概述](#rdma概述)
    - [什么是RDMA](#什么是rdma)
    - [RDMA的核心特性](#rdma的核心特性)
    - [延迟对比](#延迟对比)
    - [RDMA操作类型](#rdma操作类型)
  - [InfiniBand/RoCE/iWARP](#infinibandroceiwarp)
    - [三种RDMA实现对比](#三种rdma实现对比)
    - [InfiniBand](#infiniband)
    - [RoCE (RDMA over Converged Ethernet)](#roce-rdma-over-converged-ethernet)
    - [iWARP](#iwarp)
    - [技术选型建议](#技术选型建议)
  - [Verbs API](#verbs-api)
    - [Verbs概述](#verbs概述)
    - [核心对象层次](#核心对象层次)
    - [核心对象创建流程](#核心对象创建流程)
    - [Queue Pair类型](#queue-pair类型)
    - [工作请求示例](#工作请求示例)
  - [连接建立流程](#连接建立流程)
    - [RC连接建立步骤](#rc连接建立步骤)
    - [代码实现](#代码实现)
    - [使用CM（Connection Manager）简化](#使用cmconnection-manager简化)
  - [性能优化](#性能优化)
    - [1. 批量处理（Batching）](#1-批量处理batching)
    - [2. 内联数据（Inline）](#2-内联数据inline)
    - [3. 选择性信号（Selective Signaling）](#3-选择性信号selective-signaling)
    - [4. 无锁设计](#4-无锁设计)
    - [5. 大页内存（Huge Pages）](#5-大页内存huge-pages)
    - [6. NUMA亲和性](#6-numa亲和性)
    - [7. 自适应轮询与中断](#7-自适应轮询与中断)
    - [性能调优检查清单](#性能调优检查清单)
  - [总结](#总结)

---

## RDMA概述

### 什么是RDMA

RDMA（Remote Direct Memory Access，远程直接内存访问）是一种网络技术，允许一台计算机直接访问另一台计算机的内存，无需操作系统介入，从而大幅降低延迟和提高吞吐量。

```
┌─────────────────────────────────────────────────────────────┐
│                    传统TCP/IP通信模型                        │
├─────────────────────────────────────────────────────────────┤
│  应用层  →  内核  →  协议栈  →  NIC  →  网络  →  NIC  → ... │
│     ↑___________________________________________↓           │
│              多次内存拷贝和上下文切换                        │
└─────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────┐
│                    RDMA通信模型                              │
├─────────────────────────────────────────────────────────────┤
│  应用层  ───────────────────────────────────────→  远程内存  │
│     ↑           直接内存访问（零拷贝）                        │
└─────────────────────────────────────────────────────────────┘
```

### RDMA的核心特性

| 特性 | 说明 | 优势 |
|------|------|------|
| 零拷贝（Zero Copy） | 数据直接写入远程内存 | 减少CPU开销 |
| 内核旁路（Kernel Bypass） | 应用直接操作网卡 | 降低延迟 |
| CPU卸载（CPU Offload） | 网络协议由网卡处理 | 释放CPU资源 |
| 直接内存访问 | 远程节点的内存如同本地 | 简化编程模型 |

### 延迟对比

| 通信方式 | 典型延迟 | 适用场景 |
|----------|----------|----------|
| TCP/IP（千兆以太网） | 100+ μs | 通用数据传输 |
| TCP/IP（10GbE优化） | 10-50 μs | 企业应用 |
| RDMA（RoCEv2） | 1-5 μs | 存储网络 |
| RDMA（InfiniBand HDR） | 0.6 μs | HPC、AI训练 |

### RDMA操作类型

```c
// 1. SEND/RECV - 双边操作（类似消息传递）
// 发送方发送数据，接收方需预先发布接收请求
ibv_post_send(qp, &send_wr, ...);
ibv_post_recv(qp, &recv_wr, ...);

// 2. WRITE - 单边写操作（远程内存写入）
// 直接写入远程节点的注册内存区域
ibv_post_send(qp, &write_wr, ...);

// 3. READ - 单边读操作（远程内存读取）
// 直接从远程节点读取数据到本地
ibv_post_send(qp, &read_wr, ...);

// 4. ATOMIC - 原子操作（CAS/FAA）
// 用于实现分布式锁和计数器
ibv_post_send(qp, &atomic_wr, ...);
```

---

## InfiniBand/RoCE/iWARP

### 三种RDMA实现对比

```
┌─────────────────────────────────────────────────────────────────┐
│                    RDMA技术栈对比                               │
├─────────────────────────────────────────────────────────────────┤
│  InfiniBand          RoCEv2              iWARP                  │
│  ───────────         ───────             ─────                  │
│  ┌─────────┐         ┌─────────┐         ┌─────────┐           │
│  │  IB verbs│         │  IB verbs│         │  IW verbs│           │
│  ├─────────┤         ├─────────┤         ├─────────┤           │
│  │  IB传输层│         │  RoCEv2 │         │  DDP+RDMAP│          │
│  ├─────────┤         │  (UDP/IP)│         │  (TCP/IP) │          │
│  │  IB链路层│         ├─────────┤         ├─────────┤           │
│  │  (专用)  │         │  Ethernet│         │  Ethernet│           │
│  └─────────┘         │  + PFC/ECN│         │  + TCP   │           │
│                      └─────────┘         └─────────┘           │
└─────────────────────────────────────────────────────────────────┘
```

### InfiniBand

**特点：**

- 专为HPC设计的高性能网络
- 专用交换机和线缆（QSFP/QSFP-DD）
- 最高速率：NDR 400Gbps（2024年）
- 最低延迟：约600ns

```
InfiniBand速率演进：
SDR  (2001):  2.5 Gbps × 4 = 10 Gbps
DDR  (2005):  5 Gbps × 4 = 20 Gbps
QDR  (2008):  10 Gbps × 4 = 40 Gbps
FDR  (2011):  14 Gbps × 4 = 56 Gbps
EDR  (2014):  25 Gbps × 4 = 100 Gbps
HDR  (2018):  50 Gbps × 4 = 200 Gbps
NDR  (2021):  100 Gbps × 4 = 400 Gbps
XDR  (2024):  200 Gbps × 4 = 800 Gbps
```

### RoCE (RDMA over Converged Ethernet)

**RoCEv1 vs RoCEv2：**

| 特性 | RoCEv1 | RoCEv2 |
|------|--------|--------|
| 网络层 | 二层以太网 | 三层IP |
| 路由能力 | 不支持跨子网 | 支持路由 |
| 拥塞控制 | 依赖PFC | PFC + ECN |
| 部署复杂度 | 较低 | 中等 |

**RoCEv2协议栈：**

```
┌────────────────────────────────────┐
│         Application (Verbs)        │
├────────────────────────────────────┤
│         IB Transport (BTH)         │
├────────────────────────────────────┤
│         UDP (Dest Port 4791)       │
├────────────────────────────────────┤
│         IP (IPv4/IPv6)             │
├────────────────────────────────────┤
│         Ethernet + RoCE Header     │
└────────────────────────────────────┘
```

**RoCE部署要求：**

```bash
# 1. 开启PFC（Priority Flow Control）
# 在交换机和主机网卡上配置
mlnx_qos -i eth0 --pfc 0,0,0,1,0,0,0,0

# 2. 配置ECN（显式拥塞通知）
sysctl -w net.ipv4.tcp_ecn=1

# 3. 设置DSCP到Priority映射
echo 106 > /sys/class/net/eth0/ecn/roce_rp/override_dscp
```

### iWARP

**特点：**

- 基于TCP/IP协议栈
- 利用DDP（Direct Data Placement）和RDMAP
- 更好的路由和拥塞控制（继承TCP特性）
- 部署更简单，但性能略低于RoCE

```
iWARP协议分层：
┌────────────────────────────────────┐
│         RDMA Upper Layer Headers   │
├────────────────────────────────────┤
│         RDMAP (RDDP Protocol)      │
├────────────────────────────────────┤
│         DDP (Direct Data Placement)│
├────────────────────────────────────┤
│         MPA (Marker PDU Aligned)   │
├────────────────────────────────────┤
│         TCP                        │
├────────────────────────────────────┤
│         IP                         │
└────────────────────────────────────┘
```

### 技术选型建议

| 场景 | 推荐技术 | 理由 |
|------|----------|------|
| 新建HPC集群 | InfiniBand | 最高性能，完整生态 |
| 数据中心改造 | RoCEv2 | 复用以太网基础设施 |
| 广域RDMA | iWARP | 基于TCP，路由友好 |
| 云环境 | RoCEv2 | 主流云厂商支持 |
| 低延迟交易 | InfiniBand/HDR | 亚微秒级延迟 |

---

## Verbs API

### Verbs概述

Verbs是RDMA的编程接口标准，定义了用户态访问RDMA硬件的API。

```c
// 核心头文件
#include <infiniband/verbs.h>

// 主要API命名空间
// ibv_* : InfiniBand / RoCE 通用接口
// rdma_* : 高级连接管理接口（CM）
```

### 核心对象层次

```
┌────────────────────────────────────────────────────────────┐
│                    RDMA对象关系图                           │
├────────────────────────────────────────────────────────────┤
│                                                            │
│  ┌─────────────┐                                          │
│  │ Device (HCA)│  ←── ibv_get_device_list()               │
│  └──────┬──────┘                                          │
│         │                                                  │
│  ┌──────▼──────┐                                          │
│  │   Context   │  ←── ibv_open_device()                   │
│  └──────┬──────┘                                          │
│         │                                                  │
│  ┌──────┴──────┬────────────────┬──────────────┐         │
│  │             │                │              │          │
│  ▼             ▼                ▼              ▼          │
│ ┌────┐    ┌────────┐     ┌──────────┐   ┌──────────┐    │
│ │ PD │    │ Comp   │     │   CQ     │   │   QP     │    │
│ │    │    │ Channel│     │          │   │          │    │
│ └────┘    └────────┘     └──────────┘   └──────────┘    │
│   │                          │                │           │
│   │                          │                │           │
│   ▼                          ▼                ▼           │
│ ┌──────────────┐    ┌──────────────┐   ┌──────────────┐  │
│ │ Memory Region│    │ Work Request │   │ Work Request │  │
│ │ (MR)         │    │ (Send/Recv)  │   │ (Send/Recv)  │  │
│ └──────────────┘    └──────────────┘   └──────────────┘  │
│                                                            │
└────────────────────────────────────────────────────────────┘
```

### 核心对象创建流程

```c
// 1. 获取设备列表
struct ibv_device **dev_list = ibv_get_device_list(NULL);
struct ibv_device *device = dev_list[0];

// 2. 打开设备获取上下文
struct ibv_context *ctx = ibv_open_device(device);

// 3. 创建Protection Domain
struct ibv_pd *pd = ibv_alloc_pd(ctx);

// 4. 创建Completion Queue
struct ibv_cq *cq = ibv_create_cq(ctx, 100, NULL, NULL, 0);

// 5. 创建Queue Pair
struct ibv_qp_init_attr qp_init_attr = {
    .send_cq = cq,
    .recv_cq = cq,
    .qp_type = IBV_QPT_RC,  // Reliable Connection
    .cap = {
        .max_send_wr = 100,
        .max_recv_wr = 100,
        .max_send_sge = 1,
        .max_recv_sge = 1,
    }
};
struct ibv_qp *qp = ibv_create_qp(pd, &qp_init_attr);

// 6. 注册内存区域
void *buf = malloc(4096);
struct ibv_mr *mr = ibv_reg_mr(pd, buf, 4096,
    IBV_ACCESS_LOCAL_WRITE | IBV_ACCESS_REMOTE_READ | IBV_ACCESS_REMOTE_WRITE);
```

### Queue Pair类型

| QP类型 | 全称 | 特点 | 适用场景 |
|--------|------|------|----------|
| RC | Reliable Connection | 可靠、有序、面向连接 | 多数应用 |
| UC | Unreliable Connection | 无序、无确认 | 视频流 |
| UD | Unreliable Datagram | 无连接、单包限制 | 多播、发现 |
| XRC | Extended Reliable Connection | 多个目标共享连接 | 大规模集群 |
| DC | Dynamically Connected | 按需建立连接 | 大规模MPI |

### 工作请求示例

```c
// SEND操作
struct ibv_sge send_sge = {
    .addr = (uint64_t)send_buf,  // 本地缓冲区地址
    .length = data_len,
    .lkey = mr->lkey,            // 内存区域密钥
};

struct ibv_send_wr send_wr = {
    .wr_id = 1,
    .opcode = IBV_WR_SEND,       // SEND操作
    .send_flags = IBV_SEND_SIGNALED,
    .sg_list = &send_sge,
    .num_sge = 1,
};

ibv_post_send(qp, &send_wr, NULL);

// RDMA WRITE操作
struct ibv_send_wr write_wr = {
    .wr_id = 2,
    .opcode = IBV_WR_RDMA_WRITE,
    .send_flags = IBV_SEND_SIGNALED,
    .sg_list = &send_sge,
    .num_sge = 1,
    .wr = {
        .rdma = {
            .remote_addr = remote_addr,  // 远程内存地址
            .rkey = remote_rkey,         // 远程密钥
        }
    }
};

ibv_post_send(qp, &write_wr, NULL);
```

---

## 连接建立流程

### RC连接建立步骤

```
┌─────────────┐                              ┌─────────────┐
│   Client    │                              │   Server    │
└──────┬──────┘                              └──────┬──────┘
       │                                            │
       │  1. 创建QP (INIT状态)                       │
       │  ibv_modify_qp(IBV_QPS_INIT)               │
       │────────────────────────────────────────────>│
       │                                            │
       │  2. 交换连接信息 (GID, QP Num, PSN等)       │
       │  通过TCP/UDP带外交换                        │
       │<───────────────────────────────────────────>│
       │                                            │
       │  3. 转移到RTR (Ready To Receive)            │
       │  ibv_modify_qp(IBV_QPS_RTR)                │
       │<────────────────────────────────────────────│
       │                                            │
       │  4. 转移到RTS (Ready To Send)               │
       │  ibv_modify_qp(IBV_QPS_RTS)                │
       │────────────────────────────────────────────>│
       │                                            │
       │  5. 连接建立完成，可以开始传输数据          │
       │<═══════════════════════════════════════════>│
```

### 代码实现

```c
// 连接信息交换结构
typedef struct {
    uint64_t addr;      // 缓冲区地址
    uint32_t rkey;      // 远程密钥
    uint32_t qp_num;    // QP编号
    uint16_t lid;       // Local ID (InfiniBand)
    uint8_t  gid[16];   // Global ID (RoCEv2/IB)
    uint32_t psn;       // Packet Sequence Number
} conn_info_t;

// 1. 初始化QP到INIT状态
void init_qp(struct ibv_qp *qp) {
    struct ibv_qp_attr attr = {
        .qp_state = IBV_QPS_INIT,
        .pkey_index = 0,
        .port_num = 1,
        .qp_access_flags = IBV_ACCESS_REMOTE_READ | IBV_ACCESS_REMOTE_WRITE,
    };

    ibv_modify_qp(qp, &attr,
        IBV_QP_STATE | IBV_QP_PKEY_INDEX | IBV_QP_PORT | IBV_QP_ACCESS_FLAGS);
}

// 2. 转移到RTR状态
void init_to_rtr(struct ibv_qp *qp, conn_info_t *local, conn_info_t *remote) {
    struct ibv_qp_attr attr = {
        .qp_state = IBV_QPS_RTR,
        .path_mtu = IBV_MTU_4096,
        .dest_qp_num = remote->qp_num,
        .rq_psn = remote->psn,
        .max_dest_rd_atomic = 1,
        .min_rnr_timer = 12,
        .ah_attr = {
            .is_global = 1,
            .port_num = 1,
            .grh = {
                .dgid = *(union ibv_gid *)remote->gid,
                .sgid_index = 0,
                .hop_limit = 1,
            },
        },
    };

    ibv_modify_qp(qp, &attr,
        IBV_QP_STATE | IBV_QP_AV | IBV_QP_PATH_MTU | IBV_QP_DEST_QPN |
        IBV_QP_RQ_PSN | IBV_QP_MAX_DEST_RD_ATOMIC | IBV_QP_MIN_RNR_TIMER);
}

// 3. 转移到RTS状态
void rtr_to_rts(struct ibv_qp *qp) {
    struct ibv_qp_attr attr = {
        .qp_state = IBV_QPS_RTS,
        .timeout = 14,
        .retry_cnt = 7,
        .rnr_retry = 7,
        .sq_psn = 0,
        .max_rd_atomic = 1,
    };

    ibv_modify_qp(qp, &attr,
        IBV_QP_STATE | IBV_QP_TIMEOUT | IBV_QP_RETRY_CNT |
        IBV_QP_RNR_RETRY | IBV_QP_SQ_PSN | IBV_QP_MAX_QP_RD_ATOMIC);
}
```

### 使用CM（Connection Manager）简化

```c
// RDMA CM提供更简单的连接管理
#include <rdma/rdma_cma.h>

// 服务端
void server_listen() {
    struct rdma_cm_id *listen_id;
    struct rdma_event_channel *ec = rdma_create_event_channel();

    rdma_create_id(ec, &listen_id, NULL, RDMA_PS_TCP);
    rdma_bind_addr(listen_id, (struct sockaddr *)&server_addr);
    rdma_listen(listen_id, 10);

    // 等待连接请求
    struct rdma_cm_event *event;
    rdma_get_cm_event(ec, &event);
    if (event->event == RDMA_CM_EVENT_CONNECT_REQUEST) {
        struct rdma_cm_id *conn_id = event->id;
        // 接受连接...
        rdma_accept(conn_id, &conn_param);
    }
}

// 客户端
void client_connect() {
    struct rdma_cm_id *conn_id;
    struct rdma_event_channel *ec = rdma_create_event_channel();

    rdma_create_id(ec, &conn_id, NULL, RDMA_PS_TCP);
    rdma_resolve_addr(conn_id, NULL, (struct sockaddr *)&server_addr, 2000);
    rdma_resolve_route(conn_id, 2000);
    rdma_connect(conn_id, &conn_param);
}
```

---

## 性能优化

### 1. 批量处理（Batching）

```c
// ❌ 逐个发送 - 低效率
for (i = 0; i < count; i++) {
    ibv_post_send(qp, &wr[i], NULL);  // 每次系统调用
}

// ✅ 批量发送 - 减少系统调用
struct ibv_send_wr *bad_wr;
ibv_post_send(qp, &wr[0], &bad_wr);  // 一次发送多个WR
```

### 2. 内联数据（Inline）

```c
// 小数据直接内联，避免DMA开销
if (length <= MAX_INLINE_DATA) {
    wr.send_flags |= IBV_SEND_INLINE;  // 数据嵌入WR
}
```

### 3. 选择性信号（Selective Signaling）

```c
// ❌ 每个操作都通知 - 高CQ压力
wr.send_flags |= IBV_SEND_SIGNALED;  // 每个WR

// ✅ 周期性信号 - 批量确认
if (++send_count % SIGNAL_BATCH == 0) {
    wr.send_flags |= IBV_SEND_SIGNALED;
}

// 定期轮询CQ
if (send_count - last_polled >= SIGNAL_BATCH) {
    ibv_poll_cq(cq, BATCH_SIZE, wc);
    last_polled = send_count;
}
```

### 4. 无锁设计

```c
// 每线程私有资源，避免锁竞争
struct thread_resource {
    struct ibv_qp *qp;
    struct ibv_cq *cq;
    void *buf;
    struct ibv_mr *mr;
} __attribute__((aligned(CACHE_LINE_SIZE)));

struct thread_resource resources[NUM_THREADS];
```

### 5. 大页内存（Huge Pages）

```bash
# 系统配置大页
echo 1024 > /sys/kernel/mm/hugepages/hugepages-2048kB/nr_hugepages

# 代码中使用
#include <sys/mman.h>

void *buf = mmap(NULL, size, PROT_READ | PROT_WRITE,
    MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB, -1, 0);
```

### 6. NUMA亲和性

```c
#include <numa.h>

void numa_aware_alloc(struct ibv_context *ctx) {
    int dev_node = ibv_get_device_numa_node(ctx->device);

    // 绑定到NIC所在NUMA节点
    numa_run_on_node(dev_node);

    // 从本地节点分配内存
    void *buf = numa_alloc_onnode(size, dev_node);
}
```

### 7. 自适应轮询与中断

```c
// 忙轮询（低延迟场景）
while (1) {
    int n = ibv_poll_cq(cq, BATCH, wc);
    if (n > 0) process_completions(wc, n);

    // 偶尔让出CPU
    if (++poll_count > YIELD_THRESHOLD) {
        sched_yield();
        poll_count = 0;
    }
}

// 或使用ibv_get_cq_event（节能场景）
struct ibv_comp_channel *channel = ibv_create_comp_channel(ctx);
struct ibv_cq *cq = ibv_create_cq(ctx, 100, NULL, channel, 0);
ibv_req_notify_cq(cq, 0);  // 启用中断通知

// 等待完成事件
struct ibv_cq *ev_cq;
void *ev_ctx;
ibv_get_cq_event(channel, &ev_cq, &ev_ctx);
```

### 性能调优检查清单

| 优化项 | 配置建议 | 预期提升 |
|--------|----------|----------|
| MTU大小 | 使用4KB MTU | 吞吐量+10-20% |
| 信号频率 | 每16-64个WR | 延迟-20% |
| 内联阈值 | 64-256字节 | 小消息延迟-30% |
| 批量大小 | 16-64个WR | CPU效率+50% |
| 大页内存 | 2MB或1GB页 | TLB miss-90% |
| NUMA绑定 | NIC与内存同节点 | 延迟-10-20% |
| PFC/ECN | 启用无损以太网 | 稳定性大幅提升 |

---

## 总结

| 主题 | 核心要点 |
|------|----------|
| RDMA概述 | 零拷贝、内核旁路、CPU卸载，微秒级延迟 |
| 技术选型 | IB用于HPC，RoCEv2用于数据中心，iWARP用于路由场景 |
| Verbs API | Device → Context → PD → CQ → QP → MR 层次结构 |
| 连接建立 | INIT → RTR → RTS 三阶段状态机转换 |
| 性能优化 | 批量处理、选择性信号、内联、大页、NUMA亲和 |

> **关键原则**：RDMA性能优化需要在延迟、吞吐量和CPU效率之间找到平衡点，根据实际工作负载选择合适的策略。

> **状态**: ✅ 已完成
