/*
 * Auto-generated from: 03_System_Technology_Domains\13_RDMA_Network\02_One_Sided_RDMA.md
 * Line: 398
 * Language: c
 * Block ID: fe2db4e9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 原子FAA - 用于计数器或队列 */
int rdma_atomic_faa(struct ibv_qp *qp, uint64_t *local_addr, uint32_t lkey,
                    uint64_t remote_addr, uint32_t rkey,
                    uint64_t add_val, uint64_t wr_id) {
    struct ibv_sge sge = {
        .addr = (uint64_t)local_addr,
        .length = sizeof(uint64_t),
        .lkey = lkey,
    };

    struct ibv_send_wr wr = {
        .wr_id = wr_id,
        .opcode = IBV_WR_ATOMIC_FETCH_AND_ADD,
        .sg_list = &sge,
        .num_sge = 1,
        .wr.atomic.remote_addr = remote_addr,
        .wr.atomic.rkey = rkey,
        .wr.atomic.compare_add = add_val,  /* 增加值 */
        .send_flags = IBV_SEND_SIGNALED,
    };

    struct ibv_send_wr *bad_wr;
    return ibv_post_send(qp, &wr, &bad_wr);
}

/* MPMC队列入队 */
bool mpmc_enqueue(struct ibv_qp *qp, struct ibv_mr *mr,
                  uint64_t head_remote, uint64_t tail_remote,
                  uint64_t data_remote, void *item) {
    uint64_t ticket, tail;

    /* FAA获取ticket */
    rdma_atomic_faa(qp, &ticket, mr->lkey, head_remote, mr->rkey, 1, 1);

    /* 等待完成... */

    /* 写入数据到slot */
    uint64_t slot_addr = data_remote + (ticket % QUEUE_SIZE) * ITEM_SIZE;
    rdma_write(qp, item, ITEM_SIZE, slot_addr, mr->rkey, 2);

    return true;
}
