/*
 * Auto-generated from: 03_System_Technology_Domains\13_RDMA_Network\README.md
 * Line: 172
 * Language: c
 * Block ID: 87d1ef6a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 创建队列对
struct ibv_qp *qp;
struct ibv_qp_init_attr qp_attr = {};

qp_attr.send_cq = cq;
qp_attr.recv_cq = cq;
qp_attr.qp_type = IBV_QPT_RC; // Reliable Connection
qp_attr.sq_sig_all = 1;
qp_attr.cap.max_send_wr = 100;
qp_attr.cap.max_recv_wr = 100;
qp_attr.cap.max_send_sge = 1;
qp_attr.cap.max_recv_sge = 1;

qp = ibv_create_qp(pd, &qp_attr);
if (!qp) {
    fprintf(stderr, "Failed to create QP\n");
    return -1;
}
