/*
 * Auto-generated from: 03_System_Technology_Domains\13_RDMA_Network\01_Verbs_API.md
 * Line: 351
 * Language: c
 * Block ID: 742b630a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 发送数据 */
int rdma_send(RDMACtx *rdma, const void *data, size_t len) {
    struct ibv_sge sge = {
        .addr = (uint64_t)data,
        .length = len,
        .lkey = rdma->mr->lkey,
    };

    struct ibv_send_wr wr = {
        .wr_id = 1,
        .opcode = IBV_WR_SEND,
        .sg_list = &sge,
        .num_sge = 1,
        .send_flags = IBV_SEND_SIGNALED,  /* 产生完成事件 */
    };

    struct ibv_send_wr *bad_wr;
    if (ibv_post_send(rdma->qp, &wr, &bad_wr)) {
        perror("ibv_post_send");
        return -1;
    }

    return 0;
}

/* 接收数据 */
int rdma_recv(RDMACtx *rdma, void *buf, size_t len) {
    struct ibv_sge sge = {
        .addr = (uint64_t)buf,
        .length = len,
        .lkey = rdma->mr->lkey,
    };

    struct ibv_recv_wr wr = {
        .wr_id = 2,
        .sg_list = &sge,
        .num_sge = 1,
    };

    struct ibv_recv_wr *bad_wr;
    if (ibv_post_recv(rdma->qp, &wr, &bad_wr)) {
        perror("ibv_post_recv");
        return -1;
    }

    return 0;
}
