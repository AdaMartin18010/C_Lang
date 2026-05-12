# DPDK与RDMA：内核旁路网络编程深度指南

> **标准**: DPDK 24.11+, RDMA Verbs API (libibverbs), Linux Kernel 6.x
> **前置知识**: [网络编程基础](README.md), [内存管理](../../01_Core_Knowledge_System/02_Core_Layer/02_Memory_Management.md)
> **后续延伸**: [io_uring现代异步I/O](../05_Network_Programming/04_io_uring_Modern_Async_IO.md), [高性能数据库](../09_Database_Internals/01_B_Tree_Implementation_Case.md)
> **对标资源**: DPDK官方文档, Intel DPDK指南, Mellanox/RDMAmojo, ACM Queue
> **难度**: 5/5 | **预计学习时间**: 8-10 小时

---

## 目录

- [DPDK与RDMA：内核旁路网络编程深度指南](#dpdk与rdma内核旁路网络编程深度指南)
  - [目录](#目录)
  - [一、为什么需要内核旁路](#一为什么需要内核旁路)
    - [1.1 Linux内核网络栈的开销](#11-linux内核网络栈的开销)
    - [1.2 内核旁路的核心思想](#12-内核旁路的核心思想)
    - [1.3 三种内核旁路技术](#13-三种内核旁路技术)
  - [二、DPDK：数据平面开发套件](#二dpdk数据平面开发套件)
    - [2.1 DPDK架构](#21-dpdk架构)
    - [2.2 核心概念：mbuf与Ring](#22-核心概念mbuf与ring)
    - [2.3 Hugepage内存管理](#23-hugepage内存管理)
    - [2.4 CPU亲和性与隔离](#24-cpu亲和性与隔离)
  - [三、RDMA：远程直接内存访问](#三rdma远程直接内存访问)
    - [3.1 RDMA核心概念](#31-rdma核心概念)
    - [3.2 InfiniBand vs RoCE vs iWARP](#32-infiniband-vs-roce-vs-iwarp)
    - [3.3 Verbs API核心对象](#33-verbs-api核心对象)
  - [四、DPDK实战：最小转发应用](#四dpdk实战最小转发应用)
    - [4.1 最小DPDK程序（l2fwd风格）](#41-最小dpdk程序l2fwd风格)
  - [五、RDMA实战：单边写入](#五rdma实战单边写入)
    - [5.1 RDMA WRITE客户端](#51-rdma-write客户端)
  - [六、性能对比与选型](#六性能对比与选型)
    - [6.1 小包转发性能（64B Ethernet帧）](#61-小包转发性能64b-ethernet帧)
    - [6.2 选型决策树](#62-选型决策树)
  - [七、与io\_uring的互补关系](#七与io_uring的互补关系)
  - [八、常见陷阱](#八常见陷阱)
    - [8.1 DPDK陷阱](#81-dpdk陷阱)
    - [8.2 RDMA陷阱](#82-rdma陷阱)
  - [九、参考资源](#九参考资源)
    - [9.1 DPDK](#91-dpdk)
    - [9.2 RDMA](#92-rdma)
    - [9.3 硬件要求](#93-硬件要求)

---

## 一、为什么需要内核旁路

### 1.1 Linux内核网络栈的开销

标准Linux网络数据路径：

```
网卡(NIC)
  │  DMA → 内核ring buffer
  ▼
[内核中断处理]
  │  上下文切换到内核态 (~1.5μs)
  ▼
[网络协议栈]
  │  sk_buff分配、IP/TCP解析、路由查找、iptables
  │  典型开销: 5-15μs per packet
  ▼
[socket缓冲区]
  │  数据拷贝到用户态 (copy_to_user)
  │  开销: ~50ns/byte × MTU
  ▼
[用户程序]  ← epoll/select唤醒
  │  上下文切换回用户态 (~1.5μs)
  ▼
应用处理
```

**总开销（64B小包）**: ~10-20μs = **50K-100K packets/sec per core**

### 1.2 内核旁路的核心思想

```
网卡(NIC)
  │  DMA → 用户态内存（Hugepage）
  ▼
[用户态驱动 PMD]
  │  轮询模式，无中断，无上下文切换
  ▼
[用户态协议栈 / 直接应用处理]
  │  零拷贝，数据包直接访问
  ▼
应用处理
```

**目标**：将小包处理性能从 **100K pps/core** 提升到 **100M pps/core**。

### 1.3 三种内核旁路技术

| 技术 | 原理 | 延迟 | 吞吐 | 复杂度 | 适用场景 |
|:-----|:-----|:----:|:----:|:------:|:---------|
| **DPDK** | 用户态轮询驱动 + 用户态协议栈 | ~1μs | ~100M pps | 高 | 虚拟化、NFV、负载均衡 |
| **RDMA** | NIC直接读写远端内存 | ~1μs | ~100Gbps | 极高 | HPC、分布式存储、AI训练 |
| **io_uring** | 共享内存批量I/O | ~2-5μs | ~10M pps | 中 | 通用高吞吐、数据库 |

> **关系**：DPDK和RDMA是**真正的内核旁路**（完全绕过内核协议栈），io_uring是**内核加速**（仍使用内核协议栈，但减少syscall开销）。

---

## 二、DPDK：数据平面开发套件

### 2.1 DPDK架构

```
┌─────────────────────────────────────┐
│           应用层 (l2fwd/l3fwd)       │
├─────────────────────────────────────┤
│      DPDK库 (librte_ether, mbuf)    │
├─────────────────────────────────────┤
│      环境抽象层 (EAL)               │
│  ├── 内存管理 (Hugepage)            │
│  ├── CPU亲和性绑定                  │
│  ├── PCI设备扫描                    │
│  └── 线程/进程管理                  │
├─────────────────────────────────────┤
│      轮询模式驱动 (PMD)             │
│  ├── ixgbe (Intel 10G)              │
│  ├── i40e (Intel 40G)               │
│  ├── mlx5 (Mellanox 25/100G)        │
│  └── virtio (虚拟网卡)              │
├─────────────────────────────────────┤
│      NIC硬件队列 (RX/TX Rings)      │
└─────────────────────────────────────┘
```

### 2.2 核心概念：mbuf与Ring

```c
// DPDK的mbuf（内存缓冲区）
struct rte_mbuf {
    void *buf_addr;           // 数据缓冲区物理地址
    uint16_t data_off;        // 数据起始偏移
    uint16_t buf_len;         // 缓冲区总长度
    uint16_t data_len;        // 实际数据长度
    uint16_t port;            // 接收端口
    uint64_t ol_flags;        // 卸载标志（checksum, VLAN等）
    struct rte_mempool *pool; // 所属内存池
    // ...
};

// RX/TX Ring描述符
struct rte_eth_rxtx_ring {
    uint16_t length;          // ring大小
    volatile uint32_t *wr_idx; // 写索引（生产者）
    volatile uint32_t *rd_idx; // 读索引（消费者）
    struct rte_mbuf **mbuf;   // mbuf指针数组
};
```

### 2.3 Hugepage内存管理

```c
// DPDK依赖Hugepage（2MB或1GB页）减少TLB miss
// 系统配置
// echo 1024 > /sys/kernel/mm/hugepages/hugepages-2048kB/nr_hugepages

// DPDK初始化时自动挂载hugepage
// /dev/hugepages/ 下的文件映射到用户态

#include <rte_memory.h>

// 分配hugepage内存
void *addr = rte_malloc_socket("my_zone", size, RTE_CACHE_LINE_SIZE, socket_id);
// 或从mempool获取mbuf
struct rte_mbuf *m = rte_pktmbuf_alloc(pktmbuf_pool);
```

### 2.4 CPU亲和性与隔离

```bash
# 将CPU 2-7隔离给DPDK使用（不在内核调度中）
# /etc/default/grub: isolcpus=2,3,4,5,6,7

# DPDK启动参数
./dpdk_app -l 2-7 -n 4 -- -p 0x3
# -l: 使用的CPU核心列表
# -n: 内存通道数
# -p: 使用的端口掩码（0x3 = 端口0和1）
```

---

## 三、RDMA：远程直接内存访问

### 3.1 RDMA核心概念

RDMA允许**网卡直接读写远程主机的内存**，无需CPU参与、无需操作系统介入。

```
主机A                    网络                    主机B
┌─────────┐            ┌─────────┐            ┌─────────┐
│  应用    │◄──────────►│  应用    │            │         │
│  (Verbs) │   SEND/RECV │  (Verbs) │            │         │
├─────────┤            ├─────────┤            ├─────────┤
│  内存区  │◄──RDMA────►│  内存区  │  ← 远端直接读写 │         │
│  (MR)   │   WRITE/READ │  (MR)   │            │         │
├─────────┤            ├─────────┤            ├─────────┤
│  RDMA网卡│◄──────────►│  RDMA网卡│            │         │
│  (NIC)   │   InfiniBand │  (NIC)   │            │         │
└─────────┘   /RoCE/iWARP └─────────┘            └─────────┘
```

**三种传输方式**：

| 操作 | 说明 | CPU参与 | 适用场景 |
|:-----|:-----|:-------:|:---------|
| **SEND/RECV** | 类似socket的消息传递 | 通知 | 控制消息、小数据 |
| **RDMA WRITE** | A直接写入B的内存 | 无 | 日志追加、数据复制 |
| **RDMA READ** | A直接从B读取内存 | 无 | 缓存读取、数据获取 |

### 3.2 InfiniBand vs RoCE vs iWARP

| 技术 | 物理层 | 延迟 | 带宽 | 成本 | 适用场景 |
|:-----|:-------|:----:|:----:|:----:|:---------|
| **InfiniBand** | 专用网络 | ~600ns | 400Gbps | 高 | HPC、AI集群 |
| **RoCE v2** | 以太网 | ~1-2μs | 200Gbps | 中 | 数据中心 |
| **iWARP** | 以太网 | ~3-5μs | 100Gbps | 低 | 通用RDMA |

### 3.3 Verbs API核心对象

```c
#include <infiniband/verbs.h>

// 1. 设备上下文
struct ibv_context *ctx = ibv_open_device(device_list[0]);

// 2. 保护域（Protection Domain）
struct ibv_pd *pd = ibv_alloc_pd(ctx);

// 3. 内存注册（Memory Registration）
char *buf = malloc(BUF_SIZE);
struct ibv_mr *mr = ibv_reg_mr(pd, buf, BUF_SIZE,
    IBV_ACCESS_LOCAL_WRITE | IBV_ACCESS_REMOTE_READ | IBV_ACCESS_REMOTE_WRITE);
// mr->lkey: 本地密钥
// mr->rkey: 远程密钥（用于RDMA WRITE/READ）

// 4. 完成队列（Completion Queue）
struct ibv_cq *cq = ibv_create_cq(ctx, CQ_SIZE, NULL, NULL, 0);

// 5. 队列对（Queue Pair）
struct ibv_qp_init_attr qp_attr = {
    .send_cq = cq,
    .recv_cq = cq,
    .cap = {
        .max_send_wr = 128,
        .max_recv_wr = 128,
        .max_send_sge = 1,
        .max_recv_sge = 1
    },
    .qp_type = IBV_QPT_RC  // Reliable Connection
};
struct ibv_qp *qp = ibv_create_qp(pd, &qp_attr);
```

---

## 四、DPDK实战：最小转发应用

### 4.1 最小DPDK程序（l2fwd风格）

```c
#include <rte_ethdev.h>
#include <rte_mbuf.h>
#include <rte_eal.h>
#include <stdio.h>

#define RX_RING_SIZE 1024
#define TX_RING_SIZE 1024
#define NUM_MBUFS 8191
#define MBUF_CACHE_SIZE 250
#define BURST_SIZE 32

static struct rte_mempool *mbuf_pool;

static uint16_t port_init(uint16_t port, struct rte_mempool *mbuf_pool) {
    struct rte_eth_conf port_conf = {0};
    uint16_t nb_rxd = RX_RING_SIZE;
    uint16_t nb_txd = TX_RING_SIZE;
    int retval;

    retval = rte_eth_dev_configure(port, 1, 1, &port_conf);
    if (retval != 0) return retval;

    retval = rte_eth_dev_adjust_nb_rx_tx_desc(port, &nb_rxd, &nb_txd);
    if (retval != 0) return retval;

    // RX queue
    retval = rte_eth_rx_queue_setup(port, 0, nb_rxd,
        rte_eth_dev_socket_id(port), NULL, mbuf_pool);
    if (retval < 0) return retval;

    // TX queue
    retval = rte_eth_tx_queue_setup(port, 0, nb_txd,
        rte_eth_dev_socket_id(port), NULL);
    if (retval < 0) return retval;

    retval = rte_eth_dev_start(port);
    if (retval < 0) return retval;

    rte_eth_promiscuous_enable(port);
    return 0;
}

int main(int argc, char *argv[]) {
    unsigned nb_ports = 2;
    uint16_t portid;

    // 初始化EAL
    int ret = rte_eal_init(argc, argv);
    if (ret < 0) rte_exit(EXIT_FAILURE, "EAL init failed\n");

    argc -= ret;
    argv += ret;

    // 创建mbuf池
    mbuf_pool = rte_pktmbuf_pool_create("MBUF_POOL",
        NUM_MBUFS * nb_ports, MBUF_CACHE_SIZE, 0,
        RTE_MBUF_DEFAULT_BUF_SIZE, rte_socket_id());
    if (mbuf_pool == NULL)
        rte_exit(EXIT_FAILURE, "Cannot create mbuf pool\n");

    // 初始化端口
    RTE_ETH_FOREACH_DEV(portid) {
        if (port_init(portid, mbuf_pool) != 0)
            rte_exit(EXIT_FAILURE, "Cannot init port %u\n", portid);
    }

    if (rte_eth_dev_count_avail() < 2)
        rte_exit(EXIT_FAILURE, "Need 2 ports\n");

    printf("DPDK L2 Forwarder running...\n");

    // 主循环：轮询接收，转发到另一个端口
    for (;;) {
        RTE_ETH_FOREACH_DEV(portid) {
            struct rte_mbuf *bufs[BURST_SIZE];
            uint16_t nb_rx = rte_eth_rx_burst(portid, 0, bufs, BURST_SIZE);

            if (unlikely(nb_rx == 0)) continue;

            // 简单L2转发：发送到另一个端口
            uint16_t dst_port = portid ^ 1;
            uint16_t nb_tx = rte_eth_tx_burst(dst_port, 0, bufs, nb_rx);

            // 释放未发送的mbuf
            if (unlikely(nb_tx < nb_rx)) {
                for (uint16_t buf = nb_tx; buf < nb_rx; buf++)
                    rte_pktmbuf_free(bufs[buf]);
            }
        }
    }

    return 0;
}
```

**编译**：

```bash
# meson build
meson setup build
cd build
ninja

# 运行（需要root权限和 Hugepage）
sudo ./dpdk_l2fwd -l 0-1 -n 4 -- -p 0x3
```

---

## 五、RDMA实战：单边写入

### 5.1 RDMA WRITE客户端

```c
#include <infiniband/verbs.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUF_SIZE 4096
#define CQ_SIZE 16

struct rdma_context {
    struct ibv_context *ctx;
    struct ibv_pd *pd;
    struct ibv_cq *cq;
    struct ibv_qp *qp;
    struct ibv_mr *mr;
    char *buf;
};

int rdma_init(struct rdma_context *rdma, const char *dev_name) {
    struct ibv_device **dev_list = ibv_get_device_list(NULL);
    if (!dev_list) return -1;

    struct ibv_device *dev = NULL;
    for (int i = 0; dev_list[i]; i++) {
        if (strcmp(ibv_get_device_name(dev_list[i]), dev_name) == 0) {
            dev = dev_list[i];
            break;
        }
    }
    if (!dev) return -1;

    rdma->ctx = ibv_open_device(dev);
    ibv_free_device_list(dev_list);
    if (!rdma->ctx) return -1;

    rdma->pd = ibv_alloc_pd(rdma->ctx);
    rdma->buf = aligned_alloc(4096, BUF_SIZE);
    memset(rdma->buf, 0, BUF_SIZE);

    rdma->mr = ibv_reg_mr(rdma->pd, rdma->buf, BUF_SIZE,
        IBV_ACCESS_LOCAL_WRITE | IBV_ACCESS_REMOTE_WRITE);

    rdma->cq = ibv_create_cq(rdma->ctx, CQ_SIZE, NULL, NULL, 0);

    struct ibv_qp_init_attr qp_attr = {
        .send_cq = rdma->cq,
        .recv_cq = rdma->cq,
        .cap = {
            .max_send_wr = 1,
            .max_recv_wr = 1,
            .max_send_sge = 1,
            .max_recv_sge = 1
        },
        .qp_type = IBV_QPT_RC
    };
    rdma->qp = ibv_create_qp(rdma->pd, &qp_attr);

    return 0;
}

// 执行RDMA WRITE：将本地数据写入远端内存
int rdma_write(struct rdma_context *rdma,
               uint64_t remote_addr, uint32_t remote_rkey,
               const void *data, size_t len) {
    // 复制数据到本地注册缓冲区
    memcpy(rdma->buf, data, len);

    struct ibv_sge sge = {
        .addr = (uint64_t)rdma->buf,
        .length = (uint32_t)len,
        .lkey = rdma->mr->lkey
    };

    struct ibv_send_wr wr = {
        .wr_id = 1,
        .opcode = IBV_WR_RDMA_WRITE,  // 单边写入
        .sg_list = &sge,
        .num_sge = 1,
        .wr = {
            .rdma = {
                .remote_addr = remote_addr,
                .rkey = remote_rkey
            }
        }
    };

    struct ibv_send_wr *bad_wr;
    int ret = ibv_post_send(rdma->qp, &wr, &bad_wr);
    if (ret) return ret;

    // 等待完成
    struct ibv_wc wc;
    do {
        ret = ibv_poll_cq(rdma->cq, 1, &wc);
    } while (ret == 0);

    if (wc.status != IBV_WC_SUCCESS) {
        fprintf(stderr, "RDMA WRITE failed: %s\n",
                ibv_wc_status_str(wc.status));
        return -1;
    }

    return 0;
}

void rdma_cleanup(struct rdma_context *rdma) {
    ibv_dereg_mr(rdma->mr);
    free(rdma->buf);
    ibv_destroy_qp(rdma->qp);
    ibv_destroy_cq(rdma->cq);
    ibv_dealloc_pd(rdma->pd);
    ibv_close_device(rdma->ctx);
}

int main() {
    struct rdma_context rdma;
    if (rdma_init(&rdma, "mlx5_0") != 0) {
        fprintf(stderr, "RDMA init failed\n");
        return 1;
    }

    printf("Local buffer: addr=%p, rkey=%u\n", rdma.buf, rdma.mr->rkey);

    // 实际使用时需要交换地址和rkey信息
    // 这里仅作为示例框架

    rdma_cleanup(&rdma);
    return 0;
}
```

---

## 六、性能对比与选型

### 6.1 小包转发性能（64B Ethernet帧）

| 方案 | 延迟 | 单核吞吐 | CPU占用 |
|:-----|:----:|:--------:|:-------:|
| Linux内核 + epoll | ~20μs | ~100K pps | 100% |
| io_uring | ~5μs | ~1M pps | 80% |
| DPDK（轮询） | ~1μs | ~100M pps | 100% |
| DPDK（中断） | ~3μs | ~10M pps | 20% |
| RDMA SEND/RECV | ~1.5μs | ~50M pps | 5% |
| RDMA WRITE | ~1μs | ~线速 | <1% |

### 6.2 选型决策树

```
需要网络加速？
    │
    ├── 同机内进程间通信？
    │   ├── 是 → 共享内存（最快，0拷贝）
    │   └── 否 → 继续
    │
    ├── 需要跨机远程访问？
    │   ├── 是 → 有RDMA网卡？
    │   │   ├── 是 → RDMA（最低延迟，零CPU）
    │   │   └── 否 → 考虑RoCE/DPU
    │   └── 否 → 继续
    │
    ├── 需要处理数百万级并发连接？
    │   ├── 是 → DPDK + 用户态协议栈
    │   └── 否 → 继续
    │
    ├── 需要兼容现有socket应用？
    │   ├── 是 → io_uring（最小改动）
    │   └── 否 → DPDK
    │
    └── 一般高吞吐 → io_uring（推荐）
```

---

## 七、与io_uring的互补关系

| 场景 | 推荐方案 | 原因 |
|:-----|:---------|:-----|
| **通用高吞吐服务器** | io_uring | 无需特殊硬件，兼容现有代码 |
| **云原生NFV/VNF** | DPDK | 虚拟化优化，OVS-DPDK生态 |
| **分布式数据库** | RDMA + DPDK | 存储层RDMA，网络层DPDK |
| **AI训练集群** | RDMA (NCCL) | GPU Direct RDMA |
| **高频交易** | DPDK + kernel bypass | 亚微秒级延迟要求 |
| **Web服务器** | io_uring | 开发成本最低 |

---

## 八、常见陷阱

### 8.1 DPDK陷阱

| 陷阱 | 原因 | 解决 |
|:-----|:-----|:-----|
| **NUMA不匹配** | mbuf分配到远程NUMA节点 | 使用 `rte_malloc_socket()` 和 `rte_eth_dev_socket_id()` |
| **Cache line伪共享** | 多个核心修改同一cache line | 使用 `__rte_cache_aligned` 填充 |
| **mbuf泄漏** | 未释放不再使用的mbuf | 使用rte_mempool审计工具 |
| **Hugepage不足** | 系统未预留足够大页 | 启动前检查 `/proc/meminfo` |

### 8.2 RDMA陷阱

| 陷阱 | 原因 | 解决 |
|:-----|:-----|:-----|
| **内存未注册** | 普通内存不能被RDMA访问 | 所有通信缓冲区必须通过 `ibv_reg_mr` |
| **QP状态错误** | 连接未正确建立 | 严格遵循INIT→RTR→RTS状态机 |
| **完成队列溢出** | 未及时poll CQ | 使用多个CQ或增加CQ大小 |
| **RKey泄露** | 远程密钥被未授权访问 | 使用PSN验证和加密（RoCEv2 IPsec） |

---

## 九、参考资源

### 9.1 DPDK

| 资源 | URL |
|:-----|:----|
| DPDK官方文档 | <https://doc.dpdk.org/guides/> |
| DPDK示例程序 | dpdk/examples/ in source tree |
| 《DPDK学习笔记》 | 社区中文资料 |
| ACM Queue: "The Rise of DPDK" | ACM Queue 2018 |

### 9.2 RDMA

| 资源 | URL |
|:-----|:----|
| RDMAmojo | <https://www.rdmamojo.com/> |
| Mellanox/NVIDIA RDMA文档 | <https://docs.nvidia.com/networking/> |
| libibverbs man pages | `man ibv_reg_mr`, `man ibv_post_send` |
| 《RDMA技术详解》 | 中文社区资料 |

### 9.3 硬件要求

| 技术 | 最低硬件要求 |
|:-----|:-------------|
| DPDK | 支持SR-IOV的Intel/Mellanox网卡（推荐X710, XL710, ConnectX-4+） |
| RDMA | Mellanox ConnectX-5+（InfiniBand/RoCE）或Intel E810（iWARP） |

---

> **总结**: DPDK和RDMA代表了网络编程的**极致性能路径**。DPDK通过**用户态轮询**消除了中断和上下文切换开销，适合包处理和NFV；RDMA通过**网卡直接内存访问**实现了接近硬件极限的延迟，适合分布式存储和HPC。对于大多数应用，**io_uring**提供了更好的性价比；但对于需要**亚微秒延迟**或**线速包处理**的场景，DPDK/RDMA是不可替代的。**
