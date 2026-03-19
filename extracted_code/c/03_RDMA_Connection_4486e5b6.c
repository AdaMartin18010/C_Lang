/*
 * Auto-generated from: 03_System_Technology_Domains\03_RDMA_Connection.md
 * Line: 541
 * Language: c
 * Block ID: 4486e5b6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 每线程私有资源，避免锁竞争
struct thread_resource {
    struct ibv_qp *qp;
    struct ibv_cq *cq;
    void *buf;
    struct ibv_mr *mr;
} __attribute__((aligned(CACHE_LINE_SIZE)));

struct thread_resource resources[NUM_THREADS];
