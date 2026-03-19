/*
 * Auto-generated from: 03_System_Technology_Domains\13_RDMA_Network\01_Verbs_API.md
 * Line: 226
 * Language: c
 * Block ID: bb0f6ee1
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 创建Queue Pair */
int rdma_create_qp(RDMACtx *rdma, enum ibv_qp_type qp_type) {
    struct ibv_qp_init_attr qp_init_attr = {
        .qp_context = rdma,
        .send_cq = rdma->cq,
        .recv_cq = rdma->cq,
        .cap = {
            .max_send_wr = 128,
            .max_recv_wr = 128,
            .max_send_sge = 1,
            .max_recv_sge = 1,
            .max_inline_data = 64,
        },
        .qp_type = qp_type,  /* IBV_QPT_RC (可靠连接) 或 IBV_QPT_UD */
        .sq_sig_all = 0,     /* 只有显式设置SIGNALED的WR才产生CQE */
    };

    rdma->qp = ibv_create_qp(rdma->pd, &qp_init_attr);
    if (!rdma->qp) {
        perror("ibv_create_qp");
        return -1;
    }

    rdma->qpn = rdma->qp->qp_num;
    rdma->psn = rand() & 0xFFFFFF;  /* 24位随机数 */

    printf("Created QP: qpn=0x%x, psn=0x%x\n", rdma->qpn, rdma->psn);

    return 0;
}

/* QP状态转换 */
int rdma_modify_qp_to_init(RDMACtx *rdma) {
    struct ibv_qp_attr attr = {
        .qp_state = IBV_QPS_INIT,
        .pkey_index = 0,
        .port_num = rdma->port_num,
        .qp_access_flags = IBV_ACCESS_REMOTE_READ |
                          IBV_ACCESS_REMOTE_WRITE |
                          IBV_ACCESS_REMOTE_ATOMIC,
    };

    int flags = IBV_QP_STATE | IBV_QP_PKEY_INDEX |
                IBV_QP_PORT | IBV_QP_ACCESS_FLAGS;

    if (ibv_modify_qp(rdma->qp, &attr, flags)) {
        perror("ibv_modify_qp to INIT");
        return -1;
    }

    return 0;
}

int rdma_modify_qp_to_rtr(RDMACtx *rdma, uint16_t dlid,
                          union ibv_gid *dgid, uint32_t dqpn, uint32_t dpsn) {
    struct ibv_qp_attr attr = {
        .qp_state = IBV_QPS_RTR,
        .path_mtu = IBV_MTU_1024,
        .dest_qp_num = dqpn,
        .rq_psn = dpsn,
        .max_dest_rd_atomic = 1,
        .min_rnr_timer = 12,
    };

    /* 地址处理 */
    if (dlid) {
        /* IB模式 */
        attr.ah_attr.is_global = 0;
        attr.ah_attr.dlid = dlid;
        attr.ah_attr.sl = 0;
        attr.ah_attr.src_path_bits = 0;
        attr.ah_attr.port_num = rdma->port_num;
    } else {
        /* RoCE模式 */
        attr.ah_attr.is_global = 1;
        attr.ah_attr.grh.dgid = *dgid;
        attr.ah_attr.grh.flow_label = 0;
        attr.ah_attr.grh.sgid_index = 0;
        attr.ah_attr.grh.hop_limit = 1;
        attr.ah_attr.grh.traffic_class = 0;
        attr.ah_attr.port_num = rdma->port_num;
    }

    int flags = IBV_QP_STATE | IBV_QP_AV | IBV_QP_PATH_MTU |
                IBV_QP_DEST_QPN | IBV_QP_RQ_PSN |
                IBV_QP_MAX_DEST_RD_ATOMIC | IBV_QP_MIN_RNR_TIMER;

    if (ibv_modify_qp(rdma->qp, &attr, flags)) {
        perror("ibv_modify_qp to RTR");
        return -1;
    }

    return 0;
}

int rdma_modify_qp_to_rts(RDMACtx *rdma) {
    struct ibv_qp_attr attr = {
        .qp_state = IBV_QPS_RTS,
        .sq_psn = rdma->psn,
        .max_rd_atomic = 1,
        .retry_cnt = 7,
        .rnr_retry = 7,
        .timeout = 14,  /* 约4秒超时 */
    };

    int flags = IBV_QP_STATE | IBV_QP_SQ_PSN |
                IBV_QP_MAX_QP_RD_ATOMIC | IBV_QP_RETRY_CNT |
                IBV_QP_RNR_RETRY | IBV_QP_TIMEOUT;

    if (ibv_modify_qp(rdma->qp, &attr, flags)) {
        perror("ibv_modify_qp to RTS");
        return -1;
    }

    return 0;
}
