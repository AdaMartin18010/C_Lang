/*
 * Auto-generated from: 03_System_Technology_Domains\13_RDMA_Network\01_Verbs_API_Detailed.md
 * Line: 1432
 * Language: c
 * Block ID: 9aa77518
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * RDMA Read/Write操作实现
 */

#include <infiniband/verbs.h>
#include <string.h>

/**
 * RDMA Write操作
 */
int post_rdma_write(struct ibv_qp *qp, void *local_buf, uint32_t len,
                    uint32_t lkey, uint64_t remote_addr, uint32_t rkey,
                    int signaled) {
    struct ibv_sge sge;
    struct ibv_send_wr wr;
    struct ibv_send_wr *bad_wr;

    sge.addr = (uint64_t)local_buf;
    sge.length = len;
    sge.lkey = lkey;

    memset(&wr, 0, sizeof(wr));
    wr.wr_id = 10;
    wr.opcode = IBV_WR_RDMA_WRITE;
    wr.sg_list = &sge;
    wr.num_sge = 1;
    wr.wr.rdma.remote_addr = remote_addr;
    wr.wr.rdma.rkey = rkey;

    if (signaled) {
        wr.send_flags = IBV_SEND_SIGNALED;
    }

    /* 小数据内联优化 */
    if (len <= 256) {
        wr.send_flags |= IBV_SEND_INLINE;
    }

    int ret = ibv_post_send(qp, &wr, &bad_wr);
    if (ret) {
        fprintf(stderr, "RDMA Write failed: %d\n", ret);
        return ret;
    }

    return 0;
}

/**
 * RDMA Write with Immediate
 */
int post_rdma_write_imm(struct ibv_qp *qp, void *local_buf, uint32_t len,
                        uint32_t lkey, uint64_t remote_addr, uint32_t rkey,
                        uint32_t imm_data) {
    struct ibv_sge sge;
    struct ibv_send_wr wr;
    struct ibv_send_wr *bad_wr;

    sge.addr = (uint64_t)local_buf;
    sge.length = len;
    sge.lkey = lkey;

    memset(&wr, 0, sizeof(wr));
    wr.wr_id = 11;
    wr.opcode = IBV_WR_RDMA_WRITE_WITH_IMM;
    wr.imm_data = htonl(imm_data);
    wr.sg_list = &sge;
    wr.num_sge = 1;
    wr.wr.rdma.remote_addr = remote_addr;
    wr.wr.rdma.rkey = rkey;
    wr.send_flags = IBV_SEND_SIGNALED;

    return ibv_post_send(qp, &wr, &bad_wr);
}

/**
 * RDMA Read操作
 */
int post_rdma_read(struct ibv_qp *qp, void *local_buf, uint32_t len,
                   uint32_t lkey, uint64_t remote_addr, uint32_t rkey) {
    struct ibv_sge sge;
    struct ibv_send_wr wr;
    struct ibv_send_wr *bad_wr;

    sge.addr = (uint64_t)local_buf;
    sge.length = len;
    sge.lkey = lkey;

    memset(&wr, 0, sizeof(wr));
    wr.wr_id = 12;
    wr.opcode = IBV_WR_RDMA_READ;
    wr.sg_list = &sge;
    wr.num_sge = 1;
    wr.wr.rdma.remote_addr = remote_addr;
    wr.wr.rdma.rkey = rkey;
    wr.send_flags = IBV_SEND_SIGNALED;

    int ret = ibv_post_send(qp, &wr, &bad_wr);
    if (ret) {
        fprintf(stderr, "RDMA Read failed: %d\n", ret);
        return ret;
    }

    return 0;
}

/**
 * 多SGE RDMA Write（分散/聚集）
 */
int post_rdma_write_sge(struct ibv_qp *qp, struct ibv_sge *sg_list,
                        int num_sge, uint64_t remote_addr, uint32_t rkey) {
    struct ibv_send_wr wr;
    struct ibv_send_wr *bad_wr;

    memset(&wr, 0, sizeof(wr));
    wr.wr_id = 13;
    wr.opcode = IBV_WR_RDMA_WRITE;
    wr.sg_list = sg_list;
    wr.num_sge = num_sge;
    wr.wr.rdma.remote_addr = remote_addr;
    wr.wr.rdma.rkey = rkey;
    wr.send_flags = IBV_SEND_SIGNALED;

    return ibv_post_send(qp, &wr, &bad_wr);
}

/**
 * RDMA Read-Modify-Write 模式
 */
rdma_buffer_t* setup_rdma_buffer_pair(struct ibv_pd *pd, size_t size) {
    rdma_buffer_t *buf = rdma_register_memory(pd, size,
        IBV_ACCESS_LOCAL_WRITE |
        IBV_ACCESS_REMOTE_READ |
        IBV_ACCESS_REMOTE_WRITE);
    return buf;
}

/**
 * 双向RDMA数据传输示例
 */
int rdma_bidirectional_transfer(struct ibv_qp *qp,
                                rdma_buffer_t *local_send,
                                rdma_buffer_t *local_recv,
                                rdma_buffer_t *remote_info) {
    int ret;

    /* 1. 先Post Recv准备接收 */
    ret = post_recv(qp, local_recv->buf, local_recv->size, local_recv->lkey);
    if (ret) return ret;

    /* 2. 执行RDMA Write写入远程 */
    ret = post_rdma_write(qp, local_send->buf, local_send->size,
                          local_send->lkey,
                          remote_info->vaddr, remote_info->rkey, 1);
    if (ret) return ret;

    /* 3. 等待完成 */
    /* ... CQ轮询代码 ... */

    return 0;
}
