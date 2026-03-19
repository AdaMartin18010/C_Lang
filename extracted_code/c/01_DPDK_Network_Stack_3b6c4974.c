/*
 * Auto-generated from: 04_Industrial_Scenarios\03_High_Frequency_Trading\01_DPDK_Network_Stack.md
 * Line: 445
 * Language: c
 * Block ID: 3b6c4974
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 在NUMA节点0分配内存，在节点1使用
struct rte_mempool *pool = rte_pktmbuf_pool_create("pool",
    NUM_MBUFS, CACHE_SIZE, 0, BUF_SIZE, 0);  // socket 0

// 在socket 1的端口使用
rte_eth_rx_queue_setup(1, 0, RING_SIZE, 1, NULL, pool);  // 性能差！

// ✅ NUMA感知分配
int socket_id = rte_eth_dev_socket_id(portid);
struct rte_mempool *pool = rte_pktmbuf_pool_create("pool",
    NUM_MBUFS, CACHE_SIZE, 0, BUF_SIZE, socket_id);
