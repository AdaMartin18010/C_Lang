/*
 * Auto-generated from: 03_System_Technology_Domains\13_RDMA_Network\02_One_Sided_RDMA.md
 * Line: 164
 * Language: c
 * Block ID: 489171fc
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* Inline发送 - 小数据直接嵌入WQE，绕过注册内存 */
int rdma_write_inline(struct ibv_qp *qp, const void *data, size_t len,
                      uint64_t remote_addr, uint32_t rkey, uint64_t wr_id) {
    /* 检查inline数据大小限制 */
    struct ibv_qp_init_attr init_attr;
    struct ibv_qp_attr attr;

    if (len > 64) {  /* 典型inline限制 */
        return -1;  /* 数据太大 */
    }

    struct ibv_sge sge = {
        .addr = (uint64_t)data,
        .length = len,
        .lkey = 0,  /* inline不需要lkey */
    };

    struct ibv_send_wr wr = {
        .wr_id = wr_id,
        .opcode = IBV_WR_RDMA_WRITE,
        .sg_list = &sge,
        .num_sge = 1,
        .wr.rdma.remote_addr = remote_addr,
        .wr.rdma.rkey = rkey,
        .send_flags = IBV_SEND_SIGNALED | IBV_SEND_INLINE,
    };

    struct ibv_send_wr *bad_wr;
    return ibv_post_send(qp, &wr, &bad_wr);
}
