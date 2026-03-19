/*
 * Auto-generated from: 03_System_Technology_Domains\03_RDMA_Connection.md
 * Line: 323
 * Language: c
 * Block ID: fba536b8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// SEND操作
struct ibv_sge send_sge = {
    .addr = (uint64_t)send_buf,  // 本地缓冲区地址
    .length = data_len,
    .lkey = mr->lkey,            // 内存区域密钥
};

struct ibv_send_wr send_wr = {
    .wr_id = 1,
    .opcode = IBV_WR_SEND,       // SEND操作
    .send_flags = IBV_SEND_SIGNALED,
    .sg_list = &send_sge,
    .num_sge = 1,
};

ibv_post_send(qp, &send_wr, NULL);

// RDMA WRITE操作
struct ibv_send_wr write_wr = {
    .wr_id = 2,
    .opcode = IBV_WR_RDMA_WRITE,
    .send_flags = IBV_SEND_SIGNALED,
    .sg_list = &send_sge,
    .num_sge = 1,
    .wr = {
        .rdma = {
            .remote_addr = remote_addr,  // 远程内存地址
            .rkey = remote_rkey,         // 远程密钥
        }
    }
};

ibv_post_send(qp, &write_wr, NULL);
