# DPDK高性能网络编程

> **层级**: L4 (方法论层) + L3 (系统层)
> **目标**: 掌握DPDK内核旁路技术，实现用户态高性能网络处理
> **关联**: [高性能服务器设计](../06_High_Performance_Server/README.md) | [Linux内核模块](../03_OS_Internals/05_Linux_Kernel_Module_Programming.md)

---

## 1. DPDK概述

### 1.1 内核网络栈的性能瓶颈

```
┌─────────────────────────────────────────────────────────────┐
│              Linux内核网络栈处理流程                         │
├─────────────────────────────────────────────────────────────┤
│                                                              │
│  传统Linux网络路径 (收包):                                    │
│  ┌─────────────────────────────────────────────────────┐   │
│  │  NIC (网卡)                                         │   │
│  │    │                                                │   │
│  │    ▼ DMA (直接内存访问)                            │   │
│  │  Ring Buffer (内核)                               │   │
│  │    │                                                │   │
│  │    ▼ 硬件中断                                       │   │
│  │  CPU中断处理                                        │   │
│  │    │                                                │   │
│  │    ▼ 软中断 (NAPI polling)                          │   │
│  │  内核协议栈 (L2→L3→L4)                              │   │
│  │    │                                                │   │
│  │    ▼ 系统调用上下文切换                             │   │
│  │  用户态socket缓冲区                                 │   │
│  │    │                                                │   │
│  │    ▼ 应用层                                         │   │
│  │  Application                                        │   │
│  └─────────────────────────────────────────────────────┘   │
│                                                              │
│  性能瓶颈：                                                   │
│  1. 中断开销 - 每次收包都触发中断，上下文切换               │
│  2. 内存拷贝 - 数据从内核态拷贝到用户态                     │
│  3. 系统调用 - 用户态/内核态切换开销                        │
│  4. 锁竞争 - 多核访问共享队列                               │
│  5. Cache Miss - 数据跨越核边界                             │
│                                                              │
│  典型性能：1-3 Mpps (百万包/秒) per core                    │
│                                                              │
└─────────────────────────────────────────────────────────────┘
```

### 1.2 DPDK内核旁路架构

```
┌─────────────────────────────────────────────────────────────┐
│              DPDK内核旁路架构                                │
├─────────────────────────────────────────────────────────────┤
│                                                              │
│  DPDK处理路径 (收包):                                         │
│  ┌─────────────────────────────────────────────────────┐   │
│  │  NIC (网卡)                                         │   │
│  │    │                                                │   │
│  │    ▼ DMA (直接内存访问)                            │   │
│  │  DPDK Hugepage Memory (用户态)                    │   │
│  │    │                                                │   │
│  │    ▼ PMD (Poll Mode Driver) 轮询                   │   │
│  │  DPDK Libraries (mbuf, ring, mempool)             │   │
│  │    │                                                │   │
│  │    ▼ 用户态协议栈 (可选)                            │   │
│  │  Application (完全用户态)                          │   │
│  └─────────────────────────────────────────────────────┘   │
│                                                              │
│  DPDK核心优化：                                               │
│  ┌─────────────────────────────────────────────────────┐   │
│  │  1. 大页内存 (Hugepages)                             │   │
│  │     • 减少TLB miss                                   │   │
│  │     • 2MB或1GB大页                                   │   │
│  │                                                      │   │
│  │  2. 轮询模式驱动 (PMD)                               │   │
│  │     • 替代中断驱动                                   │   │
│  │     • 忙轮询减少延迟                                 │   │
│  │                                                      │   │
│  │  3. CPU亲和性绑定                                    │   │
│  │     • 网卡队列绑定到特定CPU核                        │   │
│  │     • 避免跨核数据移动                               │   │
│  │                                                      │   │
│  │  4. 无锁环形队列 (Lockless Rings)                    │   │
│  │     • 生产者-消费者无锁实现                          │   │
│  │     • 批量处理减少开销                               │   │
│  │                                                      │   │
│  │  5. 零拷贝 (Zero Copy)                               │   │
│  │     • 数据包直接在用户态处理                         │   │
│  │     • 避免内核-用户态拷贝                            │   │
│  └─────────────────────────────────────────────────────┘   │
│                                                              │
│  典型性能：10-100+ Mpps per core (提升10-50倍)              │
│                                                              │
└─────────────────────────────────────────────────────────────┘
```

---

## 2. DPDK核心组件

### 2.1 内存管理

```c
/* ============================================================
 * DPDK内存管理: Hugepages, Mempool, Mbuf
 * ============================================================ */

#include <rte_common.h>
#include <rte_malloc.h>
#include <rte_memory.h>
#include <rte_mempool.h>
#include <rte_mbuf.h>

/* 1. Hugepages 配置
 *
 * 启动时预留大页内存:
 * echo 1024 > /sys/kernel/mm/hugepages/hugepages-2048kB/nr_hugepages
 *
 * 挂载hugetlbfs:
 * mkdir /mnt/huge
 * mount -t hugetlbfs nodev /mnt/huge
 */

/* 2. Memory Pool (内存池) */
/* 创建固定大小的对象池，用于快速分配释放 */

struct rte_mempool *mbuf_pool;

int setup_mempool(void) {
    /* 参数: name, n (元素数), cache_size, priv_size,
     *       data_room_size, socket_id, flags */
    mbuf_pool = rte_pktmbuf_pool_create(
        "MBUF_POOL",
        8192,           /* 8192个mbuf */
        256,            /* per-core缓存大小 */
        0,              /* 私有数据大小 */
        RTE_MBUF_DEFAULT_BUF_SIZE, /* 数据缓冲区大小 */
        rte_socket_id() /* NUMA节点 */
    );

    if (!mbuf_pool) {
        rte_exit(EXIT_FAILURE, "Cannot create mbuf pool\n");
    }

    printf("Mempool created with %u elements\n",
           rte_mempool_avail_count(mbuf_pool));
    return 0;
}

/* 3. Mbuf (内存缓冲区) */
/* DPDK的数据包结构，包含元数据和数据缓冲区 */

/*
 * Mbuf结构:
 * ┌─────────────────────────────────────────────────────┐
 * │  struct rte_mbuf (元数据)                           │
 * │  - buf_addr: 数据缓冲区指针                         │
 * │  - data_off: 数据起始偏移                           │
 * │  - pkt_len: 包总长度                                │
 * │  - data_len: 段数据长度                             │
 * │  - nb_segs: 段数 (用于Scatter-Gather)               │
 * │  - next: 下一个mbuf (链式)                          │
 * ├─────────────────────────────────────────────────────┤
 * │  Headroom (RTE_PKTMBUF_HEADROOM, 默认128B)          │
 * ├─────────────────────────────────────────────────────┤
 * │  Packet Data (数据)                                 │
 * │  [可以向前扩展(Header)或向后扩展(Payload)]           │
 * ├─────────────────────────────────────────────────────┤
 * │  Tailroom (剩余空间)                                │
 * └─────────────────────────────────────────────────────┘
 */

/* Mbuf操作示例 */
void mbuf_operations(void) {
    /* 从pool分配mbuf */
    struct rte_mbuf *mbuf = rte_pktmbuf_alloc(mbuf_pool);
    if (!mbuf) {
        printf("Allocation failed\n");
        return;
    }

    /* 获取数据指针 */
    char *data = rte_pktmbuf_mtod(mbuf, char *);

    /* 追加数据 */
    char payload[] = "Hello DPDK";
    rte_pktmbuf_append(mbuf, sizeof(payload));
    memcpy(data, payload, sizeof(payload));

    /* 调整头部空间 (用于添加协议头) */
    char *eth_header = rte_pktmbuf_prepend(mbuf, 14);
    /* 填充以太网头... */

    /* 链式mbuf (大包分段) */
    struct rte_mbuf *mbuf2 = rte_pktmbuf_alloc(mbuf_pool);
    rte_pktmbuf_chain(mbuf, mbuf2);

    /* 释放mbuf */
    rte_pktmbuf_free(mbuf);
}

/* 4. 批量分配/释放 (提高性能) */
void mbuf_bulk_operations(void) {
    struct rte_mbuf *mbufs[32];

    /* 批量分配 */
    unsigned int n = rte_pktmbuf_alloc_bulk(mbuf_pool, mbufs, 32);
    if (n != 32) {
        printf("Bulk allocation failed\n");
        return;
    }

    /* 使用mbufs... */

    /* 批量释放 */
    rte_pktmbuf_free_bulk(mbufs, 32);
}
```

### 2.2 环形队列 (Ring)

```c
/* ============================================================
 * DPDK无锁环形队列
 * ============================================================ */

#include <rte_ring.h>

/* 创建无锁环形队列 */
struct rte_ring *ring;

int setup_ring(void) {
    /* 参数: name, count, flags */
    ring = rte_ring_create(
        "MY_RING",
        1024,                    /* 必须是2的幂 */
        rte_socket_id(),
        RING_F_SP_ENQ |          /* 单生产者 */
        RING_F_SC_DEQ            /* 单消费者 */
    );

    if (!ring) {
        rte_exit(EXIT_FAILURE, "Cannot create ring\n");
    }

    return 0;
}

/* Ring操作 */
void ring_operations(void) {
    void *obj;
    struct rte_mbuf *mbufs[32];

    /* 单生产者入队 */
    rte_ring_sp_enqueue(ring, obj);

    /* 单消费者出队 */
    rte_ring_sc_dequeue(ring, &obj);

    /* 批量入队 (多生产者安全) */
    unsigned int n = rte_ring_mp_enqueue_bulk(ring, (void **)mbufs, 32, NULL);

    /* 批量出队 (多消费者安全) */
    n = rte_ring_mc_dequeue_bulk(ring, (void **)mbufs, 32, NULL);

    /* Burst模式 (推荐，性能更好) */
    n = rte_ring_enqueue_burst(ring, (void **)mbufs, 32, NULL);
    n = rte_ring_dequeue_burst(ring, (void **)mbufs, 32, NULL);
}

/* Ring在多核通信中的应用 */
/*
 * Core A (RX)        Ring Queue        Core B (TX)
 *    │                   │                  │
 *    │  rte_pktmbuf_alloc│                  │
 *    ▼                   │                  │
 * ┌─────┐               │                  │
 * │mbuf │──────────────►│                  │
 * └─────┘  enqueue_burst│                  │
 *                       │                  │
 *                       │    ┌─────┐       │
 *                       └───►│mbuf │──────►│ rte_eth_tx_burst
 *                 dequeue_burst└─────┘       │
 */
```

### 2.3 PMD (轮询模式驱动)

```c
/* ============================================================
 * DPDK PMD: Poll Mode Driver
 * ============================================================ */

#include <rte_ethdev.h>

#define RX_RING_SIZE 1024
#define TX_RING_SIZE 1024
#define NUM_MBUFS 8191
#define MBUF_CACHE_SIZE 250

/* 网卡端口配置 */
static const struct rte_eth_conf port_conf_default = {
    .rxmode = {
        .max_lro_pkt_size = RTE_ETHER_MAX_LEN,
    },
};

/* 初始化网卡端口 */
int port_init(uint16_t port, struct rte_mempool *mbuf_pool) {
    struct rte_eth_conf port_conf = port_conf_default;
    uint16_t nb_rxd = RX_RING_SIZE;
    uint16_t nb_txd = TX_RING_SIZE;
    int retval;

    /* 检查端口有效性 */
    if (!rte_eth_dev_is_valid_port(port)) {
        return -1;
    }

    /* 配置RX队列 */
    retval = rte_eth_dev_configure(port, 1, 1, &port_conf);
    if (retval != 0) {
        return retval;
    }

    /* 调整描述符数量 */
    retval = rte_eth_dev_adjust_nb_rx_tx_desc(port, &nb_rxd, &nb_txd);
    if (retval != 0) {
        return retval;
    }

    /* 配置RX队列 (队列0, 每个队列1024描述符) */
    retval = rte_eth_rx_queue_setup(
        port, 0, nb_rxd,
        rte_eth_dev_socket_id(port),
        NULL, mbuf_pool
    );
    if (retval < 0) {
        return retval;
    }

    /* 配置TX队列 */
    retval = rte_eth_tx_queue_setup(
        port, 0, nb_txd,
        rte_eth_dev_socket_id(port),
        NULL
    );
    if (retval < 0) {
        return retval;
    }

    /* 启动端口 */
    retval = rte_eth_dev_start(port);
    if (retval < 0) {
        return retval;
    }

    /* 启用混杂模式 */
    rte_eth_promiscuous_enable(port);

    return 0;
}

/* 收发包主循环 */
int main_loop(uint16_t port) {
    struct rte_mbuf *bufs[BURST_SIZE];
    uint16_t nb_rx, nb_tx;

    printf("Starting main loop on port %u\n", port);

    while (1) {
        /* 批量收包 (Burst RX) */
        nb_rx = rte_eth_rx_burst(port, 0, bufs, BURST_SIZE);

        if (nb_rx == 0) {
            /* 无包时可以让出CPU或继续轮询 */
            /* rte_pause() 或 usleep(1) */
            continue;
        }

        /* 处理包 */
        for (int i = 0; i < nb_rx; i++) {
            struct rte_mbuf *mbuf = bufs[i];

            /* 获取以太网头 */
            struct rte_ether_hdr *eth_hdr = rte_pktmbuf_mtod(mbuf,
                                                struct rte_ether_hdr *);

            /* 根据协议类型处理 */
            uint16_t ether_type = rte_be_to_cpu_16(eth_hdr->ether_type);

            switch (ether_type) {
                case RTE_ETHER_TYPE_IPV4:
                    process_ipv4_packet(mbuf);
                    break;
                case RTE_ETHER_TYPE_ARP:
                    process_arp_packet(mbuf);
                    break;
                default:
                    /* 不认识的包，丢弃 */
                    rte_pktmbuf_free(mbuf);
                    bufs[i] = NULL;
                    break;
            }
        }

        /* 批量发包 */
        nb_tx = rte_eth_tx_burst(port, 0, bufs, nb_rx);

        /* 释放未发送的包 */
        if (nb_tx < nb_rx) {
            for (int i = nb_tx; i < nb_rx; i++) {
                rte_pktmbuf_free(bufs[i]);
            }
        }
    }

    return 0;
}
```

---

## 3. DPDK应用开发

### 3.1 完整示例: L2转发

```c
/* ============================================================
 * DPDK L2 Forwarder (简单二层交换机)
 * ============================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <sys/types.h>
#include <sys/queue.h>
#include <netinet/in.h>
#include <setjmp.h>
#include <stdarg.h>
#include <ctype.h>
#include <errno.h>
#include <getopt.h>
#include <signal.h>
#include <stdbool.h>

#include <rte_common.h>
#include <rte_log.h>
#include <rte_malloc.h>
#include <rte_memory.h>
#include <rte_memcpy.h>
#include <rte_eal.h>
#include <rte_launch.h>
#include <rte_atomic.h>
#include <rte_cycles.h>
#include <rte_prefetch.h>
#include <rte_lcore.h>
#include <rte_per_lcore.h>
#include <rte_branch_prediction.h>
#include <rte_interrupts.h>
#include <rte_random.h>
#include <rte_debug.h>
#include <rte_ether.h>
#include <rte_ethdev.h>
#include <rte_mempool.h>
#include <rte_mbuf.h>

#define RX_RING_SIZE 1024
#define TX_RING_SIZE 1024
#define NUM_MBUFS 8191
#define MBUF_CACHE_SIZE 250
#define BURST_SIZE 32

static volatile bool force_quit;

/* 信号处理 */
static void signal_handler(int signum) {
    if (signum == SIGINT || signum == SIGTERM) {
        printf("\n\nSignal %d received, preparing to exit...\n", signum);
        force_quit = true;
    }
}

/* L2转发处理函数 */
static void l2fwd_mac_updating(struct rte_mbuf *m, unsigned dest_portid) {
    struct rte_ether_hdr *eth;
    void *tmp;

    eth = rte_pktmbuf_mtod(m, struct rte_ether_hdr *);

    /* 交换源目MAC地址 */
    tmp = &eth->d_addr.addr_bytes[0];
    *((uint64_t *)tmp) = 0x000000000000 + ((uint64_t)dest_portid << 40);

    /* 这里应该填入真实的dest MAC地址 */
    /* 简单示例中只修改了目的MAC的最后字节作为端口标识 */
}

/* 核心处理循环 */
static int l2fwd_main_loop(void) {
    struct rte_mbuf *bufs[BURST_SIZE];
    unsigned lcore_id;

    lcore_id = rte_lcore_id();

    /* 获取此核负责的端口 */
    uint16_t port_id = 0; /* 简化示例 */

    printf("Core %u forwarding for port %u\n", lcore_id, port_id);

    while (!force_quit) {
        /* 收包 */
        const uint16_t nb_rx = rte_eth_rx_burst(port_id, 0, bufs, BURST_SIZE);

        if (nb_rx == 0) {
            continue;
        }

        /* 处理每个包 */
        for (int i = 0; i < nb_rx; i++) {
            struct rte_mbuf *mbuf = bufs[i];

            /* 确定转发端口 (简单示例: 0->1, 1->0) */
            uint16_t dst_port = (port_id == 0) ? 1 : 0;

            /* 更新MAC地址 */
            l2fwd_mac_updating(mbuf, dst_port);

            /* 发送 */
            uint16_t sent = rte_eth_tx_burst(dst_port, 0, &mbuf, 1);

            if (sent < 1) {
                rte_pktmbuf_free(mbuf);
            }
        }
    }

    return 0;
}

/* 初始化EAL */
static int dpdk_init(int argc, char **argv) {
    int ret;

    /* 初始化EAL (Environment Abstraction Layer) */
    ret = rte_eal_init(argc, argv);
    if (ret < 0) {
        rte_exit(EXIT_FAILURE, "Error with EAL initialization\n");
    }

    /* 注册信号处理 */
    force_quit = false;
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    return ret;
}

int main(int argc, char **argv) {
    struct rte_mempool *mbuf_pool;
    unsigned nb_ports;
    uint16_t portid;

    /* 初始化DPDK EAL */
    int arg_offset = dpdk_init(argc, argv);
    argc -= arg_offset;
    argv += arg_offset;

    /* 检查可用端口 */
    nb_ports = rte_eth_dev_count_avail();
    if (nb_ports < 2) {
        rte_exit(EXIT_FAILURE, "Error: at least 2 ports required\n");
    }

    /* 创建内存池 */
    mbuf_pool = rte_pktmbuf_pool_create("MBUF_POOL",
        NUM_MBUFS * nb_ports, MBUF_CACHE_SIZE, 0,
        RTE_MBUF_DEFAULT_BUF_SIZE, rte_socket_id());

    if (mbuf_pool == NULL) {
        rte_exit(EXIT_FAILURE, "Cannot create mbuf pool\n");
    }

    /* 初始化所有端口 */
    RTE_ETH_FOREACH_DEV(portid) {
        if (port_init(portid, mbuf_pool) != 0) {
            rte_exit(EXIT_FAILURE, "Cannot init port %"PRIu16 "\n", portid);
        }
    }

    /* 启动转发线程 */
    rte_eal_mp_remote_launch(l2fwd_main_loop, NULL, CALL_MAIN);

    /* 等待所有线程完成 */
    rte_eal_mp_wait_lcore();

    /* 清理 */
    RTE_ETH_FOREACH_DEV(portid) {
        rte_eth_dev_stop(portid);
        rte_eth_dev_close(portid);
    }

    /* 清理EAL */
    rte_eal_cleanup();

    return 0;
}
```

---

## 4. 总结

```
┌─────────────────────────────────────────────────────────────┐
│                                                              │
│  DPDK核心优势：                                               │
│                                                              │
│  1. 内核旁路 - 消除系统调用开销                              │
│  2. 轮询驱动 - 消除中断开销                                  │
│  3. 大页内存 - 减少TLB miss                                  │
│  4. 零拷贝 - 数据不经过内核                                  │
│  5. CPU亲和 - 避免跨核数据移动                               │
│                                                              │
│  适用场景：                                                   │
│  • 高性能NFV (虚拟路由器、防火墙、负载均衡)                  │
│  • 高频交易系统                                              │
│  • 电信核心网 (5G UPF)                                       │
│  • 包捕获与分析 (如Zeek, Suricata)                           │
│                                                              │
│  不适用场景：                                                 │
│  • 连接数少的场景 (轮询开销不划算)                           │
│  • 需要完整TCP/IP协议栈的应用                                │
│  • 开发资源有限的团队                                        │
│                                                              │
└─────────────────────────────────────────────────────────────┘
```

---

*DPDK代表了网络编程的性能极限，是构建100Gbps+网络设备的关键技术。*
