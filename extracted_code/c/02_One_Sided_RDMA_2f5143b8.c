/*
 * Auto-generated from: 03_System_Technology_Domains\13_RDMA_Network\02_One_Sided_RDMA.md
 * Line: 343
 * Language: c
 * Block ID: 2f5143b8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 原子CAS - 用于锁或无锁算法 */
int rdma_atomic_cas(struct ibv_qp *qp, uint64_t *local_addr, uint32_t lkey,
                    uint64_t remote_addr, uint32_t rkey,
                    uint64_t expected, uint64_t desired,
                    uint64_t wr_id) {
    struct ibv_sge sge = {
        .addr = (uint64_t)local_addr,
        .length = sizeof(uint64_t),
        .lkey = lkey,
    };

    struct ibv_send_wr wr = {
        .wr_id = wr_id,
        .opcode = IBV_WR_ATOMIC_CMP_AND_SWP,
        .sg_list = &sge,
        .num_sge = 1,
        .wr.atomic.remote_addr = remote_addr,
        .wr.atomic.rkey = rkey,
        .wr.atomic.compare_add = expected,    /* 比较值 */
        .wr.atomic.swap = desired,            /* 交换值 */
        .send_flags = IBV_SEND_SIGNALED,
    };

    struct ibv_send_wr *bad_wr;
    return ibv_post_send(qp, &wr, &bad_wr);
}

/* 分布式锁实现 */
typedef struct {
    uint64_t owner_id;
    uint64_t ticket;
} DistrLock;

bool acquire_lock(struct ibv_qp *qp, struct ibv_mr *mr,
                  uint64_t lock_remote_addr, uint32_t lock_rkey,
                  uint64_t my_id) {
    uint64_t local_val;

    /* CAS: 如果lock为0，设为my_id */
    rdma_atomic_cas(qp, &local_val, mr->lkey,
                   lock_remote_addr, lock_rkey,
                   0, my_id, 1);

    /* 等待完成 */
    struct ibv_wc wc;
    while (ibv_poll_cq(qp->send_cq, 1, &wc) == 0) {}

    /* local_val包含旧值 */
    return (local_val == 0);  /* 成功获取 */
}
