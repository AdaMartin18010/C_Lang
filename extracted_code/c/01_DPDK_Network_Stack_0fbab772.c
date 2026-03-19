/*
 * Auto-generated from: 04_Industrial_Scenarios\03_High_Frequency_Trading\01_DPDK_Network_Stack.md
 * Line: 376
 * Language: c
 * Block ID: 0fbab772
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// CPU亲和性设置

// 绑定线程到特定CPU核心
int bind_to_core(unsigned int core_id) {
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(core_id, &cpuset);

    return pthread_setaffinity_np(pthread_self(), sizeof(cpuset), &cpuset);
}

// 启动工作线程
void launch_worker_threads(void) {
    unsigned int lcore_id;

    // DPDK EAL已经设置了主线程亲和性

    // 为每个从核启动线程
    RTE_LCORE_FOREACH_SLAVE(lcore_id) {
        rte_eal_remote_launch(worker_loop, NULL, lcore_id);
    }
}

// 工作线程主循环
int worker_loop(void *arg) {
    uint16_t portid = 0;
    uint16_t queueid = rte_lcore_id() - 1;  // 队列绑定到核心

    printf("Worker on lcore %u, queue %u\n", rte_lcore_id(), queueid);

    while (1) {
        // 轮询接收
        receive_packets(portid, queueid);

        // 可选：每N次循环检查控制信号
        // 或者使用 rte_pause() 降低CPU占用
    }

    return 0;
}

// 预取优化
void process_with_prefetch(struct rte_mbuf **bufs, uint16_t nb_rx) {
    uint16_t i;

    for (i = 0; i < nb_rx && i < PREFETCH_OFFSET; i++) {
        process_packet(bufs[i]);
    }

    for (; i < nb_rx; i++) {
        // 预取下一个数据包的数据
        rte_prefetch0(rte_pktmbuf_mtod(bufs[i], void *));
        process_packet(bufs[i - PREFETCH_OFFSET]);
    }

    // 处理剩余的
    for (i = nb_rx - PREFETCH_OFFSET; i < nb_rx; i++) {
        process_packet(bufs[i]);
    }
}
