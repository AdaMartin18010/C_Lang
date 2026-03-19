/*
 * Auto-generated from: 03_System_Technology_Domains\13_RDMA_Network\01_Verbs_API_Detailed.md
 * Line: 1137
 * Language: c
 * Block ID: 8e0e189c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * RDMA连接建立与管理
 */

#include <infiniband/verbs.h>
#include <stdint.h>
#include <string.h>

/* 连接信息交换结构 */
typedef struct {
    uint32_t qpn;           /* QP编号 */
    uint32_t psn;           /* 包序列号 */
    uint16_t lid;           /* 本地标识符 */
    uint8_t gid[16];        /* 全局标识符 */
    uint32_t rkey;          /* 远程密钥 */
    uint64_t vaddr;         /* 虚拟地址 */
} conn_info_t;

/* 连接上下文 */
typedef struct {
    struct ibv_qp *qp;
    conn_info_t local;
    conn_info_t remote;
    int is_connected;
} rdma_conn_t;

/**
 * 准备本地连接信息
 */
void prepare_local_conn_info(rdma_conn_t *conn, struct ibv_qp *qp,
                             struct ibv_port_attr *port_attr,
                             struct ibv_gid *gid,
                             rdma_buffer_t *buf) {
    conn->qp = qp;
    conn->local.qpn = qp->qp_num;
    conn->local.psn = rand() & 0xffffff;
    conn->local.lid = port_attr->lid;
    if (gid) {
        memcpy(conn->local.gid, gid->raw, 16);
    }
    if (buf) {
        conn->local.rkey = buf->rkey;
        conn->local.vaddr = (uint64_t)buf->buf;
    }
}

/**
 * 建立RC连接
 */
int establish_rc_connection(rdma_conn_t *conn, int port_num,
                            conn_info_t *remote_info) {
    conn->remote = *remote_info;

    /* 转换QP状态 */
    int ret = qp_transition_init(conn->qp, port_num, 0);
    if (ret) return ret;

    ret = qp_transition_rtr(conn->qp, port_num,
                            remote_info->qpn,
                            remote_info->lid,
                            remote_info->gid,
                            remote_info->psn);
    if (ret) return ret;

    ret = qp_transition_rts(conn->qp, conn->local.psn);
    if (ret) return ret;

    conn->is_connected = 1;
    printf("RC连接建立成功\n");
    return 0;
}

/**
 * UD连接设置（无连接模式）
 */
struct ibv_ah* setup_ud_address_handle(struct ibv_pd *pd, int port_num,
                                       uint16_t dlid, uint8_t *dgid) {
    struct ibv_ah_attr ah_attr;

    memset(&ah_attr, 0, sizeof(ah_attr));
    ah_attr.dlid = dlid;
    ah_attr.sl = 0;
    ah_attr.src_path_bits = 0;
    ah_attr.port_num = port_num;

    if (dgid) {
        ah_attr.is_global = 1;
        memcpy(ah_attr.grh.dgid.raw, dgid, 16);
        ah_attr.grh.sgid_index = 0;
        ah_attr.grh.hop_limit = 1;
    }

    struct ibv_ah *ah = ibv_create_ah(pd, &ah_attr);
    if (!ah) {
        perror("ibv_create_ah");
        return NULL;
    }

    return ah;
}

/**
 * 断开连接
 */
void disconnect_qp(rdma_conn_t *conn) {
    if (!conn->is_connected) return;

    struct ibv_qp_attr attr;
    memset(&attr, 0, sizeof(attr));
    attr.qp_state = IBV_QPS_RESET;

    ibv_modify_qp(conn->qp, &attr, IBV_QP_STATE);
    conn->is_connected = 0;

    printf("连接已断开\n");
}
