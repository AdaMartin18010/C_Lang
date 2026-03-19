/*
 * Auto-generated from: README.md
 * Line: 875
 * Language: c
 * Block ID: 8fe04efa
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// RDMA one-sided read - 用于分布式系统
#include <infiniband/verbs.h>

void rdma_read(struct rdma_context* ctx,
               uint64_t remote_addr,
               uint32_t rkey,
               void* local_buf,
               size_t len) {
    struct ibv_sge sge = {
        .addr = (uint64_t)local_buf,
        .length = len,
        .lkey = ctx->mr->lkey
    };

    struct ibv_send_wr wr = {
        .opcode = IBV_WR_RDMA_READ,  // 单边读操作
        .wr.rdma.remote_addr = remote_addr,
        .wr.rdma.rkey = rkey,
        .num_sge = 1,
        .sg_list = &sge
    };

    ibv_post_send(ctx->qp, &wr, NULL);  // 无需远程CPU参与
}
