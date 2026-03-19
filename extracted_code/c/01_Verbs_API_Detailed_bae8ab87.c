/*
 * Auto-generated from: 03_System_Technology_Domains\13_RDMA_Network\01_Verbs_API_Detailed.md
 * Line: 880
 * Language: c
 * Block ID: bae8ab87
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 队列对(QP)创建与状态转换
 */

#include <infiniband/verbs.h>
#include <string.h>

/* QP类型与传输服务 */
typedef enum {
    QP_TYPE_RC = IBV_QPT_RC,  /* 可靠连接 */
    QP_TYPE_UC = IBV_QPT_UC,  /* 不可靠连接 */
    QP_TYPE_UD = IBV_QPT_UD,  /* 不可靠数据报 */
    QP_TYPE_RD = IBV_QPT_RD,  /* 可靠数据报 */
} qp_type_t;

/**
 * 创建队列对
 */
struct ibv_qp* create_qp(struct ibv_pd *pd, struct ibv_cq *cq,
                         uint32_t max_send_wr, uint32_t max_recv_wr,
                         enum ibv_qp_type qp_type) {
    struct ibv_qp_init_attr qp_init_attr;

    memset(&qp_init_attr, 0, sizeof(qp_init_attr));

    qp_init_attr.qp_type = qp_type;
    qp_init_attr.sq_sig_all = 0;  /* 只有设置了SIGNAL的WR才产生完成事件 */
    qp_init_attr.send_cq = cq;
    qp_init_attr.recv_cq = cq;

    qp_init_attr.cap.max_send_wr = max_send_wr;
    qp_init_attr.cap.max_recv_wr = max_recv_wr;
    qp_init_attr.cap.max_send_sge = 1;
    qp_init_attr.cap.max_recv_sge = 1;
    qp_init_attr.cap.max_inline_data = 256;  /* 内联数据大小 */

    struct ibv_qp *qp = ibv_create_qp(pd, &qp_init_attr);
    if (!qp) {
        perror("ibv_create_qp");
        return NULL;
    }

    printf("QP创建成功，编号: %u\n", qp->qp_num);
    printf("  最大发送WR: %u\n", qp_init_attr.cap.max_send_wr);
    printf("  最大接收WR: %u\n", qp_init_attr.cap.max_recv_wr);

    return qp;
}

/**
 * 转换QP到INIT状态
 */
int qp_transition_init(struct ibv_qp *qp, int port_num, int pkey_idx) {
    struct ibv_qp_attr attr;
    int flags;

    memset(&attr, 0, sizeof(attr));

    attr.qp_state = IBV_QPS_INIT;
    attr.pkey_index = pkey_idx;
    attr.port_num = port_num;
    attr.qp_access_flags = IBV_ACCESS_LOCAL_WRITE |
                           IBV_ACCESS_REMOTE_READ |
                           IBV_ACCESS_REMOTE_WRITE |
                           IBV_ACCESS_REMOTE_ATOMIC;

    flags = IBV_QP_STATE | IBV_QP_PKEY_INDEX | IBV_QP_PORT | IBV_QP_ACCESS_FLAGS;

    int ret = ibv_modify_qp(qp, &attr, flags);
    if (ret) {
        perror("ibv_modify_qp (INIT)");
        return ret;
    }

    printf("QP %u 转换到INIT状态成功\n", qp->qp_num);
    return 0;
}

/**
 * 转换QP到RTR状态
 */
int qp_transition_rtr(struct ibv_qp *qp, int port_num,
                      uint32_t dest_qp_num, uint16_t dlid,
                      uint8_t *dgid, uint32_t psn) {
    struct ibv_qp_attr attr;
    int flags;

    memset(&attr, 0, sizeof(attr));

    attr.qp_state = IBV_QPS_RTR;
    attr.path_mtu = IBV_MTU_1024;
    attr.dest_qp_num = dest_qp_num;
    attr.rq_psn = psn;
    attr.max_dest_rd_atomic = 16;
    attr.min_rnr_timer = 12;

    /* 主路径 */
    attr.ah_attr.is_global = 0;
    attr.ah_attr.dlid = dlid;
    attr.ah_attr.sl = 0;
    attr.ah_attr.src_path_bits = 0;
    attr.ah_attr.port_num = port_num;

    /* GID设置（用于RoCE） */
    if (dgid) {
        attr.ah_attr.is_global = 1;
        attr.ah_attr.grh.dgid.global.subnet_prefix =
            ((uint64_t)dgid[0] << 56) | ((uint64_t)dgid[1] << 48) |
            ((uint64_t)dgid[2] << 40) | ((uint64_t)dgid[3] << 32) |
            ((uint64_t)dgid[4] << 24) | ((uint64_t)dgid[5] << 16) |
            ((uint64_t)dgid[6] << 8) | (uint64_t)dgid[7];
        attr.ah_attr.grh.dgid.global.interface_id =
            ((uint64_t)dgid[8] << 56) | ((uint64_t)dgid[9] << 48) |
            ((uint64_t)dgid[10] << 40) | ((uint64_t)dgid[11] << 32) |
            ((uint64_t)dgid[12] << 24) | ((uint64_t)dgid[13] << 16) |
            ((uint64_t)dgid[14] << 8) | (uint64_t)dgid[15];
        attr.ah_attr.grh.sgid_index = 0;
        attr.ah_attr.grh.hop_limit = 1;
    }

    flags = IBV_QP_STATE | IBV_QP_AV | IBV_QP_PATH_MTU | IBV_QP_DEST_QPN |
            IBV_QP_RQ_PSN | IBV_QP_MAX_DEST_RD_ATOMIC | IBV_QP_MIN_RNR_TIMER;

    int ret = ibv_modify_qp(qp, &attr, flags);
    if (ret) {
        perror("ibv_modify_qp (RTR)");
        return ret;
    }

    printf("QP %u 转换到RTR状态成功 (dest_qpn=%u)\n", qp->qp_num, dest_qp_num);
    return 0;
}

/**
 * 转换QP到RTS状态
 */
int qp_transition_rts(struct ibv_qp *qp, uint32_t psn) {
    struct ibv_qp_attr attr;
    int flags;

    memset(&attr, 0, sizeof(attr));

    attr.qp_state = IBV_QPS_RTS;
    attr.timeout = 14;           /* 4.096μs * 2^14 ≈ 67ms */
    attr.retry_cnt = 7;          /* 重试次数 */
    attr.rnr_retry = 7;          /* RNR重试次数 */
    attr.sq_psn = psn;
    attr.max_rd_atomic = 16;     /* 最大未完成RDMA Read/Atomic */

    flags = IBV_QP_STATE | IBV_QP_TIMEOUT | IBV_QP_RETRY_CNT |
            IBV_QP_RNR_RETRY | IBV_QP_SQ_PSN | IBV_QP_MAX_QP_RD_ATOMIC;

    int ret = ibv_modify_qp(qp, &attr, flags);
    if (ret) {
        perror("ibv_modify_qp (RTS)");
        return ret;
    }

    printf("QP %u 转换到RTS状态成功\n", qp->qp_num);
    return 0;
}

/**
 * 完整QP初始化流程
 */
int qp_full_init(struct ibv_qp *qp, int port_num, uint32_t dest_qp_num,
                 uint16_t dlid, uint8_t *dgid) {
    int ret;
    uint32_t psn = rand() & 0xffffff;  /* 24位PSN */

    /* RST -> INIT */
    ret = qp_transition_init(qp, port_num, 0);
    if (ret) return ret;

    /* INIT -> RTR */
    ret = qp_transition_rtr(qp, port_num, dest_qp_num, dlid, dgid, psn);
    if (ret) return ret;

    /* RTR -> RTS */
    ret = qp_transition_rts(qp, psn);
    if (ret) return ret;

    printf("QP初始化完成，PSN: 0x%06x\n", psn);
    return 0;
}

/**
 * 查询QP状态
 */
void query_qp_state(struct ibv_qp *qp) {
    struct ibv_qp_attr attr;
    struct ibv_qp_init_attr init_attr;

    if (ibv_query_qp(qp, &attr, IBV_QP_STATE, &init_attr)) {
        perror("ibv_query_qp");
        return;
    }

    const char *state_str;
    switch (attr.qp_state) {
        case IBV_QPS_RESET: state_str = "RESET"; break;
        case IBV_QPS_INIT: state_str = "INIT"; break;
        case IBV_QPS_RTR: state_str = "RTR"; break;
        case IBV_QPS_RTS: state_str = "RTS"; break;
        case IBV_QPS_SQD: state_str = "SQD"; break;
        case IBV_QPS_SQE: state_str = "SQE"; break;
        case IBV_QPS_ERR: state_str = "ERR"; break;
        default: state_str = "UNKNOWN"; break;
    }

    printf("QP %u 当前状态: %s\n", qp->qp_num, state_str);
}
