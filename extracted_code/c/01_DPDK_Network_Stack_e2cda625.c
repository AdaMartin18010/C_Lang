/*
 * Auto-generated from: 04_Industrial_Scenarios\03_High_Frequency_Trading\01_DPDK_Network_Stack.md
 * Line: 327
 * Language: c
 * Block ID: e2cda625
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 自定义内存池（基于DPDK rte_mempool）

// 创建环形队列（无锁）
struct rte_ring *create_packet_ring(const char *name, unsigned count) {
    struct rte_ring *ring = rte_ring_create(
        name,
        count,
        rte_socket_id(),
        RING_F_SP_ENQ | RING_F_SC_DEQ  // 单生产者单消费者模式
    );

    if (ring == NULL) {
        rte_exit(EXIT_FAILURE, "Ring create failed: %s\n", rte_strerror(rte_errno));
    }

    return ring;
}

// 多生产者单消费者队列
void packet_enqueue(struct rte_ring *ring, struct rte_mbuf *mbuf) {
    // 非阻塞入队
    while (rte_ring_mp_enqueue(ring, mbuf) != 0) {
        // 队列满，自旋等待
        rte_pause();
    }
}

struct rte_mbuf *packet_dequeue(struct rte_ring *ring) {
    struct rte_mbuf *mbuf;

    // 非阻塞出队
    if (rte_ring_sc_dequeue(ring, (void **)&mbuf) == 0) {
        return mbuf;
    }

    return NULL;  // 队列空
}

// 批量出队提高效率
unsigned packet_dequeue_burst(struct rte_ring *ring,
                               struct rte_mbuf **mbufs,
                               unsigned max_count) {
    return rte_ring_sc_dequeue_burst(ring, (void **)mbufs, max_count, NULL);
}
