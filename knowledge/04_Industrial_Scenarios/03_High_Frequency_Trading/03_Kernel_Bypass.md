# 内核旁路网络技术

> **主题**: DPDK/RDMA内核旁路 | **难度**: L5 | **更新**: 2026-03-15

---

## 概述

内核旁路技术允许用户态程序直接访问网络硬件，避免传统内核网络栈的高延迟。

```text
传统网络栈 vs 内核旁路:
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
传统方式:                    内核旁路:
应用 → socket → TCP/IP    →  应用 → DPDK/RDMA → 网卡
       ↓ 内核空间                ↓ 用户态
       ↓ 系统调用                ↓ 零拷贝
       ↓ 数据拷贝                ↓ 微秒级延迟
       ↓ 上下文切换
网卡 → 驱动 → 内核

延迟对比:
- 传统: 10-100 微秒
- DPDK: 1-5 微秒
- RDMA: 0.5-2 微秒
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
```

## 核心概念

### 1. DPDK (Data Plane Development Kit)

Intel开发的快速数据包处理框架。

```c
// DPDK核心组件
#include <rte_eal.h>
#include <rte_ethdev.h>
#include <rte_mbuf.h>

// DPDK初始化
int dpdk_init(int argc, char** argv) {
    // 初始化Environment Abstraction Layer
    int ret = rte_eal_init(argc, argv);
    if (ret < 0) rte_exit(EXIT_FAILURE, "EAL init failed\n");

    // 获取可用端口数
    uint16_t nb_ports = rte_eth_dev_count_avail();
    printf("Available ports: %u\n", nb_ports);

    return 0;
}

// 内存池配置
struct rte_mempool* create_mempool(void) {
    return rte_pktmbuf_pool_create(
        "MBUF_POOL",
        8192,           // 缓冲区数量
        256,            // 缓存大小
        0,
        RTE_MBUF_DEFAULT_BUF_SIZE,
        rte_socket_id()
    );
}
```

### 2. RDMA (Remote Direct Memory Access)

允许网卡直接读写远程内存，无需CPU参与。

```c
// RDMA基本流程
// 1. 建立连接
// 2. 注册内存区域
// 3. 执行RDMA操作
// 4. 完成通知

// RDMA Verbs API
#include <infiniband/verbs.h>

// 创建RDMA连接
struct ibv_context* create_rdma_context(void) {
    struct ibv_device** dev_list = ibv_get_device_list(NULL);
    if (!dev_list) return NULL;

    struct ibv_context* ctx = ibv_open_device(dev_list[0]);
    ibv_free_device_list(dev_list);

    return ctx;
}

// 注册内存区域
struct ibv_mr* register_memory(struct ibv_pd* pd, void* addr, size_t len) {
    return ibv_reg_mr(
        pd,
        addr,
        len,
        IBV_ACCESS_LOCAL_WRITE |
        IBV_ACCESS_REMOTE_READ |
        IBV_ACCESS_REMOTE_WRITE
    );
}
```

## 性能对比

| 技术 | 延迟 | 吞吐量 | CPU占用 | 复杂度 |
|:-----|:-----|:-------|:--------|:-------|
| 传统Socket | 10-100μs | 1-10Gbps | 高 | 低 |
| DPDK | 1-5μs | 10-100Gbps | 中 | 中 |
| RDMA | 0.5-2μs | 100Gbps+ | 极低 | 高 |
| FPGA | <1μs | 100Gbps+ | 无 | 极高 |

## 实现示例

```c
// DPDK简单接收示例
static inline void process_packet(struct rte_mbuf* mbuf) {
    struct rte_ether_hdr* eth_hdr = rte_pktmbuf_mtod(mbuf, struct rte_ether_hdr*);

    // 快速解析
    uint16_t eth_type = rte_be_to_cpu_16(eth_hdr->ether_type);

    if (eth_type == RTE_ETHER_TYPE_IPV4) {
        // 处理IPv4包
        struct rte_ipv4_hdr* ip_hdr = (struct rte_ipv4_hdr*)(eth_hdr + 1);

        if (ip_hdr->next_proto_id == IPPROTO_UDP) {
            // 处理UDP
            struct rte_udp_hdr* udp_hdr = (struct rte_udp_hdr*)((char*)ip_hdr +
                ((ip_hdr->version_ihl & 0x0F) * 4));

            uint16_t dst_port = rte_be_to_cpu_16(udp_hdr->dst_port);

            // 高频交易逻辑在这里
            if (dst_port == HTF_TRADING_PORT) {
                process_trading_packet(udp_hdr + 1);
            }
        }
    }

    rte_pktmbuf_free(mbuf);
}
```

## 硬件要求

- **网卡**: Intel 82599/X710, Mellanox ConnectX-5/6
- **CPU**: 支持DDIO (Data Direct I/O)
- **内存**: 大页内存配置
- **PCIe**: Gen3 x8 或更高

---

> **状态**: ✅ 已完成


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
