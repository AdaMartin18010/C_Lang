/*
 * Auto-generated from: 04_Industrial_Scenarios\03_High_Frequency_Trading\README.md
 * Line: 370
 * Language: c
 * Block ID: 84dddbbf
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * DPDK 性能优化技术
 */

/* 1. CPU亲和性和隔离 */
void setup_cpu_affinity(void)
{
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);

    /* 绑定到隔离的CPU核心 */
    CPU_SET(2, &cpuset);  /* 使用核心2 */
    CPU_SET(3, &cpuset);  /* 使用核心3 */

    pthread_t current_thread = pthread_self();
    pthread_setaffinity_np(current_thread, sizeof(cpu_set_t), &cpuset);

    /* 设置实时调度策略 */
    struct sched_param param;
    param.sched_priority = sched_get_priority_max(SCHED_FIFO);
    pthread_setschedparam(current_thread, SCHED_FIFO, &param);
}

/* 2. NUMA感知内存分配 */
struct rte_mempool *create_numa_mempool(int socket_id)
{
    char name[32];
    snprintf(name, sizeof(name), "MBUF_POOL_%d", socket_id);

    return rte_pktmbuf_pool_create(
        name,
        NUM_MBUFS,
        MBUF_CACHE_SIZE,
        0,
        RTE_MBUF_DEFAULT_BUF_SIZE,
        socket_id  /* 在指定NUMA节点分配 */
    );
}

/* 3. 预取优化 */
void process_with_prefetch(struct rte_mbuf **bufs, uint16_t nb_rx)
{
    const int PREFETCH_OFFSET = 4;

    /* 预取未来数据包 */
    for (int i = 0; i < PREFETCH_OFFSET && i < nb_rx; i++) {
        rte_prefetch0(rte_pktmbuf_mtod(bufs[i], void *));
    }

    for (int i = 0; i < nb_rx; i++) {
        /* 预取下一个 */
        if (i + PREFETCH_OFFSET < nb_rx) {
            rte_prefetch0(rte_pktmbuf_mtod(bufs[i + PREFETCH_OFFSET],
                                            void *));
        }

        /* 处理当前 */
        process_packet(bufs[i]);
    }
}

/* 4. SIMD加速解析 */
#include <immintrin.h>

void fast_parse_batch_sse(struct rte_mbuf **bufs, uint16_t count)
{
    for (uint16_t i = 0; i < count; i += 4) {
        /* 使用SSE同时处理4个数据包头部 */
        __m128i headers[4];

        for (int j = 0; j < 4 && (i + j) < count; j++) {
            headers[j] = _mm_loadu_si128(
                (__m128i *)rte_pktmbuf_mtod(bufs[i + j], void *)
            );
        }

        /* 并行比较以太网类型 */
        __m128i ip4_mask = _mm_set1_epi16(RTE_ETHER_TYPE_IPV4);
        /* ... SIMD处理 ... */
    }
}
