/*
 * Auto-generated from: 03_System_Technology_Domains\13_RDMA_Network\02_One_Sided_RDMA.md
 * Line: 237
 * Language: c
 * Block ID: ecadfd71
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 使用Immediate Data进行轻量级通知 */
typedef enum {
    IMM_DATA_SYNC = 1,
    IMM_DATA_COMMIT = 2,
    IMM_DATA_ACK = 3,
} ImmDataType;

/* 发送带立即数的数据 */
int rdma_write_with_imm(struct ibv_qp *qp, void *local_buf, size_t len,
                        uint64_t remote_addr, uint32_t rkey,
                        uint32_t imm_data, uint64_t wr_id) {
    struct ibv_sge sge = {
        .addr = (uint64_t)local_buf,
        .length = len,
        .lkey = 0,  /* 假设已注册 */
    };

    struct ibv_send_wr wr = {
        .wr_id = wr_id,
        .opcode = IBV_WR_RDMA_WRITE_WITH_IMM,
        .sg_list = &sge,
        .num_sge = 1,
        .imm_data = htonl(imm_data),
        .wr.rdma.remote_addr = remote_addr,
        .wr.rdma.rkey = rkey,
        .send_flags = IBV_SEND_SIGNALED,
    };

    struct ibv_send_wr *bad_wr;
    return ibv_post_send(qp, &wr, &bad_wr);
}

/* 处理接收到的Immediate Data */
void handle_imm_data(struct ibv_wc *wc) {
    uint32_t imm = ntohl(wc->imm_data);

    switch (imm) {
    case IMM_DATA_SYNC:
        printf("Received SYNC notification\n");
        /* 处理同步点 */
        break;
    case IMM_DATA_COMMIT:
        printf("Received COMMIT notification\n");
        /* 处理提交确认 */
        break;
    }
}
