/*
 * Auto-generated from: 04_Industrial_Scenarios\03_High_Frequency_Trading\03_Kernel_Bypass.md
 * Line: 35
 * Language: c
 * Block ID: 202a6f05
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

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
