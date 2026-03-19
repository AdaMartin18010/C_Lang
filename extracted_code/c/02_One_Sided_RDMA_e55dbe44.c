/*
 * Auto-generated from: 03_System_Technology_Domains\13_RDMA_Network\02_One_Sided_RDMA.md
 * Line: 199
 * Language: c
 * Block ID: e55dbe44
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 多SGE写入 - 分散/聚集 */
int rdma_write_scatter(struct ibv_qp *qp, struct ibv_sge *sg_list, int num_sge,
                       uint64_t remote_addr, uint32_t rkey, uint64_t wr_id) {
    struct ibv_send_wr wr = {
        .wr_id = wr_id,
        .opcode = IBV_WR_RDMA_WRITE,
        .sg_list = sg_list,
        .num_sge = num_sge,
        .wr.rdma.remote_addr = remote_addr,
        .wr.rdma.rkey = rkey,
        .send_flags = IBV_SEND_SIGNALED,
    };

    struct ibv_send_wr *bad_wr;
    return ibv_post_send(qp, &wr, &bad_wr);
}

/* 示例：从多个buffer写入 */
int multi_buffer_write(struct ibv_qp *qp, struct ibv_mr *mr,
                       void *buf1, size_t len1,
                       void *buf2, size_t len2,
                       uint64_t remote_addr, uint32_t rkey) {
    struct ibv_sge sge[2] = {
        { (uint64_t)buf1, len1, mr->lkey },
        { (uint64_t)buf2, len2, mr->lkey },
    };

    return rdma_write_scatter(qp, sge, 2, remote_addr, rkey, 1);
}
