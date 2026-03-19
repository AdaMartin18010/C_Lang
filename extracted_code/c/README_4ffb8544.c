/*
 * Auto-generated from: 03_System_Technology_Domains\13_RDMA_Network\README.md
 * Line: 197
 * Language: c
 * Block ID: 4ffb8544
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 发送操作
struct ibv_sge sg;
struct ibv_send_wr wr = {};
struct ibv_send_wr *bad_wr;

sg.addr = (uint64_t)send_buf;
sg.length = data_len;
sg.lkey = mr->lkey;

wr.wr_id = 1;
wr.opcode = IBV_WR_SEND;
wr.sg_list = &sg;
wr.num_sge = 1;
wr.send_flags = IBV_SEND_SIGNALED;

ret = ibv_post_send(qp, &wr, &bad_wr);
if (ret) {
    fprintf(stderr, "Failed to post send\n");
    return -1;
}

// 接收操作
struct ibv_recv_wr recv_wr = {};
struct ibv_recv_wr *bad_recv_wr;
struct ibv_sge recv_sg;

recv_sg.addr = (uint64_t)recv_buf;
recv_sg.length = buf_size;
recv_sg.lkey = recv_mr->lkey;

recv_wr.wr_id = 2;
recv_wr.sg_list = &recv_sg;
recv_wr.num_sge = 1;

ret = ibv_post_recv(qp, &recv_wr, &bad_recv_wr);
if (ret) {
    fprintf(stderr, "Failed to post recv\n");
    return -1;
}
