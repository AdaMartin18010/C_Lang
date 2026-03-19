/*
 * Auto-generated from: 03_System_Technology_Domains\13_RDMA_Network\README.md
 * Line: 241
 * Language: c
 * Block ID: 83cf214a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// RDMA 写操作 - 直接写入远程内存
struct ibv_sge write_sg;
struct ibv_send_wr write_wr = {};

write_sg.addr = (uint64_t)local_buf;
write_sg.length = data_len;
write_sg.lkey = local_mr->lkey;

write_wr.wr_id = 3;
write_wr.opcode = IBV_WR_RDMA_WRITE;
write_wr.sg_list = &write_sg;
write_wr.num_sge = 1;
write_wr.wr.rdma.remote_addr = remote_addr;
write_wr.wr.rdma.rkey = remote_rkey;
write_wr.send_flags = IBV_SEND_SIGNALED;

ret = ibv_post_send(qp, &write_wr, &bad_wr);
