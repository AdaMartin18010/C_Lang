/*
 * Auto-generated from: 03_System_Technology_Domains\13_RDMA_Network\README.md
 * Line: 377
 * Language: c
 * Block ID: 98c76e97
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 预分配内存池
#define POOL_SIZE 1024
#define CHUNK_SIZE 4096

typedef struct {
    void *buf;
    struct ibv_mr *mr;
    int in_use;
} mempool_entry;

mempool_entry mempool[POOL_SIZE];

void init_mempool(struct ibv_pd *pd) {
    for (int i = 0; i < POOL_SIZE; i++) {
        posix_memalign(&mempool[i].buf, 4096, CHUNK_SIZE);
        mempool[i].mr = ibv_reg_mr(pd, mempool[i].buf,
                                   CHUNK_SIZE,
                                   IBV_ACCESS_LOCAL_WRITE |
                                   IBV_ACCESS_REMOTE_WRITE);
        mempool[i].in_use = 0;
    }
}
