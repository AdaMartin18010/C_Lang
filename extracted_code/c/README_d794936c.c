/*
 * Auto-generated from: 04_Industrial_Scenarios\03_High_Frequency_Trading\README.md
 * Line: 102
 * Language: c
 * Block ID: d794936c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * DPDK 初始化与基本数据包处理
 * 编译: gcc -o dpdk_example dpdk_example.c $(pkg-config --cflags --libs libdpdk)
 */

#include <rte_eal.h>
#include <rte_ethdev.h>
#include <rte_mbuf.h>
#include <rte_mempool.h>
#include <rte_cycles.h>
#include <rte_lcore.h>
#include <rte_log.h>

/* DPDK配置参数 */
#define NUM_RX_QUEUES       8
#define NUM_TX_QUEUES       8
#define RX_RING_SIZE        4096
#define TX_RING_SIZE        4096
#define NUM_MBUFS           65535
#define MBUF_CACHE_SIZE     256
#define BURST_SIZE          32

/* 端口配置 */
static const struct rte_eth_conf port_conf = {
    .rxmode = {
        .max_rx_pkt_len = RTE_ETHER_MAX_LEN,
        .split_hdr_size = 0,
        .offloads = DEV_RX_OFFLOAD_CHECKSUM,  /* 硬件校验和卸载 */
    },
    .txmode = {
        .offloads = DEV_TX_OFFLOAD_IPV4_CKSUM |
                    DEV_TX_OFFLOAD_UDP_CKSUM |
                    DEV_TX_OFFLOAD_TCP_CKSUM,
    },
};

/* DPDK初始化 */
int dpdk_init(int argc, char **argv)
{
    int ret;

    /* 初始化环境抽象层(EAL) */
    ret = rte_eal_init(argc, argv);
    if (ret < 0) {
        rte_exit(EXIT_FAILURE, "Error with EAL initialization\n");
    }

    /* 获取可用端口数 */
    uint16_t nb_ports = rte_eth_dev_count_avail();
    if (nb_ports == 0) {
        rte_exit(EXIT_FAILURE, "No Ethernet ports available\n");
    }

    RTE_LOG(INFO, USER1, "Found %u ports\n", nb_ports);
    return ret;
}

/* 内存池创建 */
struct rte_mempool *create_mempool(void)
{
    struct rte_mempool *mbuf_pool;

    /* 创建MBUF内存池 */
    mbuf_pool = rte_pktmbuf_pool_create(
        "MBUF_POOL",
        NUM_MBUFS,
        MBUF_CACHE_SIZE,
        0,                          /* 私有数据大小 */
        RTE_MBUF_DEFAULT_BUF_SIZE,  /* 数据缓冲区大小 */
        rte_socket_id()             /* NUMA节点 */
    );

    if (mbuf_pool == NULL) {
        rte_exit(EXIT_FAILURE, "Cannot create mbuf pool\n");
    }

    return mbuf_pool;
}

/* 端口初始化 */
int port_init(uint16_t port, struct rte_mempool *mbuf_pool)
{
    struct rte_eth_conf conf = port_conf;
    struct rte_eth_txconf txconf;
    struct rte_eth_rxconf rxconf;
    struct rte_eth_dev_info dev_info;
    int retval;
    uint16_t q;

    /* 获取设备信息 */
    rte_eth_dev_info_get(port, &dev_info);

    /* 配置RX队列 */
    rxconf = dev_info.default_rxconf;
    rxconf.offloads = conf.rxmode.offloads;

    for (q = 0; q < NUM_RX_QUEUES; q++) {
        retval = rte_eth_rx_queue_setup(
            port, q, RX_RING_SIZE,
            rte_eth_dev_socket_id(port),
            &rxconf,
            mbuf_pool
        );
        if (retval < 0) {
            rte_exit(EXIT_FAILURE, "RX queue setup failed\n");
        }
    }

    /* 配置TX队列 */
    txconf = dev_info.default_txconf;
    txconf.offloads = conf.txmode.offloads;

    for (q = 0; q < NUM_TX_QUEUES; q++) {
        retval = rte_eth_tx_queue_setup(
            port, q, TX_RING_SIZE,
            rte_eth_dev_socket_id(port),
            &txconf
        );
        if (retval < 0) {
            rte_exit(EXIT_FAILURE, "TX queue setup failed\n");
        }
    }

    /* 启动设备 */
    retval = rte_eth_dev_start(port);
    if (retval < 0) {
        rte_exit(EXIT_FAILURE, "Device start failed\n");
    }

    /* 启用混杂模式 */
    rte_eth_promiscuous_enable(port);

    return 0;
}

/* 数据包接收与处理循环 */
int packet_processing_loop(uint16_t port_id)
{
    struct rte_mbuf *bufs[BURST_SIZE];
    uint16_t nb_rx, nb_tx;
    uint16_t queue_id = 0;  /* 使用队列0 */

    RTE_LOG(INFO, USER1, "Starting processing on lcore %u\n",
            rte_lcore_id());

    while (1) {
        /* 批量接收数据包 - 轮询模式，无中断 */
        nb_rx = rte_eth_rx_burst(port_id, queue_id, bufs, BURST_SIZE);

        if (nb_rx == 0) {
            /* 可选：CPU退让或忙等待 */
            // rte_pause();  /* 减少功耗 */
            continue;
        }

        /* 处理每个数据包 */
        for (int i = 0; i < nb_rx; i++) {
            struct rte_mbuf *mbuf = bufs[i];

            /* 直接访问数据（零拷贝） */
            struct rte_ether_hdr *eth_hdr = rte_pktmbuf_mtod(mbuf,
                                                             struct rte_ether_hdr *);

            /* 解析以太网类型 */
            uint16_t eth_type = rte_be_to_cpu_16(eth_hdr->ether_type);

            if (eth_type == RTE_ETHER_TYPE_IPV4) {
                /* 处理IPv4数据包 */
                struct rte_ipv4_hdr *ip_hdr = (struct rte_ipv4_hdr *)(eth_hdr + 1);

                if (ip_hdr->next_proto_id == IPPROTO_UDP) {
                    /* 处理UDP数据包（市场数据常用） */
                    struct rte_udp_hdr *udp_hdr = (struct rte_udp_hdr *)(
                        (char *)ip_hdr + ((ip_hdr->version_ihl & 0x0F) * 4));

                    uint16_t dst_port = rte_be_to_cpu_16(udp_hdr->dst_port);

                    /* 检查是否是市场数据端口 */
                    if (dst_port == MARKET_DATA_PORT) {
                        /* 解析ITCH/OUCH/SBE协议 */
                        void *payload = (void *)(udp_hdr + 1);
                        size_t payload_len = rte_be_to_cpu_16(udp_hdr->dgram_len)
                                             - sizeof(struct rte_udp_hdr);

                        process_market_data(payload, payload_len);
                    }
                }
            }

            /* 释放MBUF */
            rte_pktmbuf_free(mbuf);
        }
    }

    return 0;
}

/* 发送订单 */
int send_order(uint16_t port_id, const void *order_data, size_t len)
{
    struct rte_mbuf *pkt;
    struct rte_ether_hdr *eth_hdr;
    struct rte_ipv4_hdr *ip_hdr;
    struct rte_tcp_hdr *tcp_hdr;
    char *payload;

    /* 分配MBUF */
    pkt = rte_pktmbuf_alloc(tx_mbuf_pool);
    if (!pkt) {
        return -ENOMEM;
    }

    /* 预留头部空间 */
    eth_hdr = (struct rte_ether_hdr *)rte_pktmbuf_append(pkt,
                                                          sizeof(*eth_hdr));
    ip_hdr = (struct rte_ipv4_hdr *)rte_pktmbuf_append(pkt,
                                                        sizeof(*ip_hdr));
    tcp_hdr = (struct rte_tcp_hdr *)rte_pktmbuf_append(pkt,
                                                        sizeof(*tcp_hdr));
    payload = (char *)rte_pktmbuf_append(pkt, len);

    /* 填充以太网头部 */
    rte_ether_addr_copy(&dst_mac, &eth_hdr->dst_addr);
    rte_ether_addr_copy(&src_mac, &eth_hdr->src_addr);
    eth_hdr->ether_type = rte_cpu_to_be_16(RTE_ETHER_TYPE_IPV4);

    /* 填充IP头部 */
    ip_hdr->version_ihl = 0x45;
    ip_hdr->type_of_service = 0;
    ip_hdr->total_length = rte_cpu_to_be_16(sizeof(*ip_hdr) +
                                             sizeof(*tcp_hdr) + len);
    ip_hdr->packet_id = 0;
    ip_hdr->fragment_offset = 0;
    ip_hdr->time_to_live = 64;
    ip_hdr->next_proto_id = IPPROTO_TCP;
    ip_hdr->hdr_checksum = 0;  /* 硬件计算 */
    ip_hdr->src_addr = src_ip;
    ip_hdr->dst_addr = dst_ip;

    /* 填充TCP头部 */
    tcp_hdr->src_port = rte_cpu_to_be_16(src_port);
    tcp_hdr->dst_port = rte_cpu_to_be_16(dst_port);
    tcp_hdr->sent_seq = rte_cpu_to_be_32(seq_num++);
    tcp_hdr->recv_ack = rte_cpu_to_be_32(ack_num);
    tcp_hdr->data_off = (sizeof(*tcp_hdr) / 4) << 4;
    tcp_hdr->tcp_flags = RTE_TCP_PSH_FLAG | RTE_TCP_ACK_FLAG;
    tcp_hdr->rx_win = rte_cpu_to_be_16(65535);
    tcp_hdr->cksum = 0;  /* 硬件计算 */
    tcp_hdr->tcp_urp = 0;

    /* 复制FIX/SBE订单数据 */
    memcpy(payload, order_data, len);

    /* 发送 */
    uint16_t nb_tx = rte_eth_tx_burst(port_id, 0, &pkt, 1);

    if (nb_tx == 0) {
        rte_pktmbuf_free(pkt);
        return -EIO;
    }

    return 0;
}
