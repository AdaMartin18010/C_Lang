/*
 * Auto-generated from: 03_System_Technology_Domains\13_RDMA_Network\01_Verbs_API.md
 * Line: 464
 * Language: c
 * Block ID: 4902ed26
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* RDMA Read - 从远程内存读取 */
int rdma_read(RDMACtx *rdma, void *local_buf, size_t len,
              uint64_t remote_addr, uint32_t rkey) {
    struct ibv_sge sge = {
        .addr = (uint64_t)local_buf,
        .length = len,
        .lkey = rdma->mr->lkey,
    };

    struct ibv_send_wr wr = {
        .wr_id = 5,
        .opcode = IBV_WR_RDMA_READ,
        .sg_list = &sge,
        .num_sge = 1,
        .wr.rdma.remote_addr = remote_addr,
        .wr.rdma.rkey = rkey,
        .send_flags = IBV_SEND_SIGNALED,
    };

    struct ibv_send_wr *bad_wr;
    if (ibv_post_send(rdma->qp, &wr, &bad_wr)) {
        perror("ibv_post_send RDMA_READ");
        return -1;
    }

    return 0;
}
