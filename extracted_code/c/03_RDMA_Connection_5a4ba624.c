/*
 * Auto-generated from: 03_System_Technology_Domains\03_RDMA_Connection.md
 * Line: 501
 * Language: c
 * Block ID: 5a4ba624
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 逐个发送 - 低效率
for (i = 0; i < count; i++) {
    ibv_post_send(qp, &wr[i], NULL);  // 每次系统调用
}

// ✅ 批量发送 - 减少系统调用
struct ibv_send_wr *bad_wr;
ibv_post_send(qp, &wr[0], &bad_wr);  // 一次发送多个WR
