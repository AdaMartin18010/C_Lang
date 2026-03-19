/*
 * Auto-generated from: 03_System_Technology_Domains\13_RDMA_Network\README.md
 * Line: 263
 * Language: c
 * Block ID: f7195f2a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// RDMA 读操作 - 直接读取远程内存
struct ibv_sge read_sg;
struct ibv_send_wr read_wr = {};

read_sg.addr = (uint64_t)local_buf;
read_sg.length = data_len;
read_sg.lkey = local_mr->lkey;

read_wr.wr_id = 4;
read_wr.opcode = IBV_WR_RDMA_READ;
read_wr.sg_list = &read_sg;
read_wr.num_sge = 1;
read_wr.wr.rdma.remote_addr = remote_addr;
read_wr.wr.rdma.rkey = remote_rkey;
read_wr.send_flags = IBV_SEND_SIGNALED;

ret = ibv_post_send(qp, &read_wr, &bad_wr);
