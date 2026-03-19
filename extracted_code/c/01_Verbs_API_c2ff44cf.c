/*
 * Auto-generated from: 03_System_Technology_Domains\13_RDMA_Network\01_Verbs_API.md
 * Line: 403
 * Language: c
 * Block ID: c2ff44cf
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* RDMA Write - 直接写入远程内存 */
int rdma_write(RDMACtx *rdma, void *local_data, size_t len,
               uint64_t remote_addr, uint32_t rkey) {
    struct ibv_sge sge = {
        .addr = (uint64_t)local_data,
        .length = len,
        .lkey = rdma->mr->lkey,
    };

    struct ibv_send_wr wr = {
        .wr_id = 3,
        .opcode = IBV_WR_RDMA_WRITE,
        .sg_list = &sge,
        .num_sge = 1,
        .wr.rdma.remote_addr = remote_addr,
        .wr.rdma.rkey = rkey,
        .send_flags = IBV_SEND_SIGNALED,
    };

    struct ibv_send_wr *bad_wr;
    if (ibv_post_send(rdma->qp, &wr, &bad_wr)) {
        perror("ibv_post_send RDMA_WRITE");
        return -1;
    }

    return 0;
}

/* RDMA Write With Immediate */
int rdma_write_imm(RDMACtx *rdma, void *local_data, size_t len,
                   uint64_t remote_addr, uint32_t rkey, uint32_t imm_data) {
    struct ibv_sge sge = {
        .addr = (uint64_t)local_data,
        .length = len,
        .lkey = rdma->mr->lkey,
    };

    struct ibv_send_wr wr = {
        .wr_id = 4,
        .opcode = IBV_WR_RDMA_WRITE_WITH_IMM,
        .sg_list = &sge,
        .num_sge = 1,
        .imm_data = htonl(imm_data),
        .wr.rdma.remote_addr = remote_addr,
        .wr.rdma.rkey = rkey,
        .send_flags = IBV_SEND_SIGNALED,
    };

    struct ibv_send_wr *bad_wr;
    if (ibv_post_send(rdma->qp, &wr, &bad_wr)) {
        perror("ibv_post_send RDMA_WRITE_WITH_IMM");
        return -1;
    }

    return 0;
}
