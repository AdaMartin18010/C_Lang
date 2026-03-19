/*
 * Auto-generated from: 03_System_Technology_Domains\13_RDMA_Network\README.md
 * Line: 342
 * Language: c
 * Block ID: 25df94cb
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 批量发送多个 WR
struct ibv_send_wr wrs[BATCH_SIZE];
struct ibv_sge sgs[BATCH_SIZE];

for (int i = 0; i < BATCH_SIZE; i++) {
    sgs[i].addr = (uint64_t)bufs[i];
    sgs[i].length = buf_sizes[i];
    sgs[i].lkey = mrs[i]->lkey;

    wrs[i].wr_id = i;
    wrs[i].opcode = IBV_WR_SEND;
    wrs[i].sg_list = &sgs[i];
    wrs[i].num_sge = 1;
    wrs[i].send_flags = (i == BATCH_SIZE - 1) ?
                        IBV_SEND_SIGNALED : 0;
    wrs[i].next = (i < BATCH_SIZE - 1) ? &wrs[i + 1] : NULL;
}

ibv_post_send(qp, &wrs[0], &bad_wr);
