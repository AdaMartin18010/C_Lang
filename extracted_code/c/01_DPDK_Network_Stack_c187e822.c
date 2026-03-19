/*
 * Auto-generated from: 04_Industrial_Scenarios\03_High_Frequency_Trading\01_DPDK_Network_Stack.md
 * Line: 124
 * Language: c
 * Block ID: c187e822
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// DPDK EAL (Environment Abstraction Layer) 初始化

#include <rte_eal.h>
#include <rte_ethdev.h>
#include <rte_mempool.h>
#include <rte_mbuf.h>

// 初始化DPDK
int dpdk_init(int argc, char **argv) {
    int ret;

    // 1. EAL初始化
    ret = rte_eal_init(argc, argv);
    if (ret < 0) {
        rte_exit(EXIT_FAILURE, "EAL initialization failed\n");
    }

    // 2. 检查可用端口
    uint16_t nb_ports = rte_eth_dev_count_avail();
    if (nb_ports == 0) {
        rte_exit(EXIT_FAILURE, "No Ethernet ports available\n");
    }

    printf("DPDK initialized. Available ports: %u\n", nb_ports);

    // 3. 创建内存池
    struct rte_mempool *mbuf_pool = rte_pktmbuf_pool_create(
        "MBUF_POOL",                    // 名称
        NUM_MBUFS * nb_ports,           // 元素数量
        MBUF_CACHE_SIZE,                // 缓存大小
        0,                              // 私有数据大小
        RTE_MBUF_DEFAULT_BUF_SIZE,      // 数据缓冲区大小
        rte_socket_id()                 // NUMA节点
    );

    if (mbuf_pool == NULL) {
        rte_exit(EXIT_FAILURE, "Cannot create mbuf pool\n");
    }

    // 4. 初始化每个端口
    for (uint16_t portid = 0; portid < nb_ports; portid++) {
        init_port(portid, mbuf_pool);
    }

    return 0;
}

// 端口初始化
int init_port(uint16_t portid, struct rte_mempool *mbuf_pool) {
    struct rte_eth_conf port_conf = {0};
    struct rte_eth_dev_info dev_info;

    // 获取设备信息
    rte_eth_dev_info_get(portid, &dev_info);

    // 配置接收队列
    port_conf.rxmode.max_rx_pkt_len = RTE_ETHER_MAX_LEN;
    port_conf.rxmode.mq_mode = ETH_MQ_RX_RSS;  // RSS多队列

    // 配置RSS
    port_conf.rx_adv_conf.rss_conf.rss_key = NULL;
    port_conf.rx_adv_conf.rss_conf.rss_hf =
        ETH_RSS_IP | ETH_RSS_TCP | ETH_RSS_UDP;

    // 配置端口
    int ret = rte_eth_dev_configure(portid, RX_RINGS, TX_RINGS, &port_conf);
    if (ret != 0) {
        rte_exit(EXIT_FAILURE, "Port %u configuration failed\n", portid);
    }

    // 设置接收队列
    for (uint16_t q = 0; q < RX_RINGS; q++) {
        ret = rte_eth_rx_queue_setup(
            portid, q, RX_RING_SIZE,
            rte_eth_dev_socket_id(portid),
            NULL, mbuf_pool
        );
        if (ret < 0) {
            rte_exit(EXIT_FAILURE, "RX queue setup failed\n");
        }
    }

    // 设置发送队列
    for (uint16_t q = 0; q < TX_RINGS; q++) {
        ret = rte_eth_tx_queue_setup(
            portid, q, TX_RING_SIZE,
            rte_eth_dev_socket_id(portid),
            NULL
        );
        if (ret < 0) {
            rte_exit(EXIT_FAILURE, "TX queue setup failed\n");
        }
    }

    // 启动端口
    ret = rte_eth_dev_start(portid);
    if (ret < 0) {
        rte_exit(EXIT_FAILURE, "Port start failed\n");
    }

    // 启用混杂模式
    rte_eth_promiscuous_enable(portid);

    return 0;
}
