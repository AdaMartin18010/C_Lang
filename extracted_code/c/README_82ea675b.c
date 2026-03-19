/*
 * Auto-generated from: 03_System_Technology_Domains\13_RDMA_Network\README.md
 * Line: 157
 * Language: c
 * Block ID: 82ea675b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 创建完成队列
struct ibv_cq *cq;
cq = ibv_create_cq(ctx, 100, NULL, NULL, 0);
if (!cq) {
    fprintf(stderr, "Failed to create CQ\n");
    return -1;
}

// 请求完成通知
ibv_req_notify_cq(cq, 0);
