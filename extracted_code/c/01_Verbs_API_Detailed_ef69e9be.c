/*
 * Auto-generated from: 03_System_Technology_Domains\13_RDMA_Network\01_Verbs_API_Detailed.md
 * Line: 1270
 * Language: c
 * Block ID: ef69e9be
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * Send/Recv操作实现
 */

#include <infiniband/verbs.h>
#include <string.h>

/**
 * Post Send操作
 */
int post_send(struct ibv_qp *qp, void *buf, uint32_t len, uint32_t lkey,
              int signaled) {
    struct ibv_sge sge;
    struct ibv_send_wr wr;
    struct ibv_send_wr *bad_wr;

    /* 设置散集元素 */
    sge.addr = (uint64_t)buf;
    sge.length = len;
    sge.lkey = lkey;

    /* 设置发送WR */
    memset(&wr, 0, sizeof(wr));
    wr.wr_id = 1;  /* 用户标识 */
    wr.opcode = IBV_WR_SEND;
    wr.sg_list = &sge;
    wr.num_sge = 1;

    if (signaled) {
        wr.send_flags = IBV_SEND_SIGNALED;
    }

    /* 内联小数据优化 */
    if (len <= 256) {
        wr.send_flags |= IBV_SEND_INLINE;
    }

    int ret = ibv_post_send(qp, &wr, &bad_wr);
    if (ret) {
        fprintf(stderr, "ibv_post_send failed: %d\n", ret);
        return ret;
    }

    return 0;
}

/**
 * Post Recv操作
 */
int post_recv(struct ibv_qp *qp, void *buf, uint32_t len, uint32_t lkey) {
    struct ibv_sge sge;
    struct ibv_recv_wr wr;
    struct ibv_recv_wr *bad_wr;

    sge.addr = (uint64_t)buf;
    sge.length = len;
    sge.lkey = lkey;

    memset(&wr, 0, sizeof(wr));
    wr.wr_id = 2;
    wr.sg_list = &sge;
    wr.num_sge = 1;

    int ret = ibv_post_recv(qp, &wr, &bad_wr);
    if (ret) {
        fprintf(stderr, "ibv_post_recv failed: %d\n", ret);
        return ret;
    }

    return 0;
}

/**
 * 批量Post Recv
 */
int post_recv_batch(struct ibv_qp *qp, rdma_buffer_t **bufs, int count) {
    struct ibv_recv_wr wrs[16];
    struct ibv_sge sges[16];
    struct ibv_recv_wr *bad_wr;
    int i;

    if (count > 16) count = 16;

    for (i = 0; i < count; i++) {
        sges[i].addr = (uint64_t)bufs[i]->buf;
        sges[i].length = bufs[i]->size;
        sges[i].lkey = bufs[i]->lkey;

        wrs[i].wr_id = i + 100;
        wrs[i].sg_list = &sges[i];
        wrs[i].num_sge = 1;
        wrs[i].next = (i < count - 1) ? &wrs[i + 1] : NULL;
    }

    return ibv_post_recv(qp, wrs, &bad_wr);
}

/**
 * 带即时数据的Send
 */
int post_send_with_imm(struct ibv_qp *qp, void *buf, uint32_t len,
                       uint32_t lkey, uint32_t imm_data) {
    struct ibv_sge sge;
    struct ibv_send_wr wr;
    struct ibv_send_wr *bad_wr;

    sge.addr = (uint64_t)buf;
    sge.length = len;
    sge.lkey = lkey;

    memset(&wr, 0, sizeof(wr));
    wr.wr_id = 3;
    wr.opcode = IBV_WR_SEND_WITH_IMM;
    wr.imm_data = htonl(imm_data);
    wr.sg_list = &sge;
    wr.num_sge = 1;
    wr.send_flags = IBV_SEND_SIGNALED;

    return ibv_post_send(qp, &wr, &bad_wr);
}

/**
 * UD模式发送
 */
int post_send_ud(struct ibv_qp *qp, struct ibv_ah *ah, uint32_t remote_qpn,
                 void *buf, uint32_t len, uint32_t lkey) {
    struct ibv_sge sge;
    struct ibv_send_wr wr;
    struct ibv_send_wr *bad_wr;

    sge.addr = (uint64_t)buf;
    sge.length = len + 40;  /* UD需要额外的40字节GRH头 */
    sge.lkey = lkey;

    memset(&wr, 0, sizeof(wr));
    wr.wr_id = 4;
    wr.opcode = IBV_WR_SEND;
    wr.sg_list = &sge;
    wr.num_sge = 1;
    wr.wr.ud.ah = ah;
    wr.wr.ud.remote_qpn = remote_qpn;
    wr.wr.ud.remote_qkey = 0x11111111;  /* 默认QKey */

    return ibv_post_send(qp, &wr, &bad_wr);
}
