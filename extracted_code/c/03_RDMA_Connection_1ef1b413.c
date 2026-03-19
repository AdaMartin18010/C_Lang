/*
 * Auto-generated from: 03_System_Technology_Domains\03_RDMA_Connection.md
 * Line: 392
 * Language: c
 * Block ID: 1ef1b413
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 连接信息交换结构
typedef struct {
    uint64_t addr;      // 缓冲区地址
    uint32_t rkey;      // 远程密钥
    uint32_t qp_num;    // QP编号
    uint16_t lid;       // Local ID (InfiniBand)
    uint8_t  gid[16];   // Global ID (RoCEv2/IB)
    uint32_t psn;       // Packet Sequence Number
} conn_info_t;

// 1. 初始化QP到INIT状态
void init_qp(struct ibv_qp *qp) {
    struct ibv_qp_attr attr = {
        .qp_state = IBV_QPS_INIT,
        .pkey_index = 0,
        .port_num = 1,
        .qp_access_flags = IBV_ACCESS_REMOTE_READ | IBV_ACCESS_REMOTE_WRITE,
    };

    ibv_modify_qp(qp, &attr,
        IBV_QP_STATE | IBV_QP_PKEY_INDEX | IBV_QP_PORT | IBV_QP_ACCESS_FLAGS);
}

// 2. 转移到RTR状态
void init_to_rtr(struct ibv_qp *qp, conn_info_t *local, conn_info_t *remote) {
    struct ibv_qp_attr attr = {
        .qp_state = IBV_QPS_RTR,
        .path_mtu = IBV_MTU_4096,
        .dest_qp_num = remote->qp_num,
        .rq_psn = remote->psn,
        .max_dest_rd_atomic = 1,
        .min_rnr_timer = 12,
        .ah_attr = {
            .is_global = 1,
            .port_num = 1,
            .grh = {
                .dgid = *(union ibv_gid *)remote->gid,
                .sgid_index = 0,
                .hop_limit = 1,
            },
        },
    };

    ibv_modify_qp(qp, &attr,
        IBV_QP_STATE | IBV_QP_AV | IBV_QP_PATH_MTU | IBV_QP_DEST_QPN |
        IBV_QP_RQ_PSN | IBV_QP_MAX_DEST_RD_ATOMIC | IBV_QP_MIN_RNR_TIMER);
}

// 3. 转移到RTS状态
void rtr_to_rts(struct ibv_qp *qp) {
    struct ibv_qp_attr attr = {
        .qp_state = IBV_QPS_RTS,
        .timeout = 14,
        .retry_cnt = 7,
        .rnr_retry = 7,
        .sq_psn = 0,
        .max_rd_atomic = 1,
    };

    ibv_modify_qp(qp, &attr,
        IBV_QP_STATE | IBV_QP_TIMEOUT | IBV_QP_RETRY_CNT |
        IBV_QP_RNR_RETRY | IBV_QP_SQ_PSN | IBV_QP_MAX_QP_RD_ATOMIC);
}
