/*
 * Auto-generated from: 03_System_Technology_Domains\03_RDMA_Connection.md
 * Line: 584
 * Language: c
 * Block ID: f9f176d8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 忙轮询（低延迟场景）
while (1) {
    int n = ibv_poll_cq(cq, BATCH, wc);
    if (n > 0) process_completions(wc, n);

    // 偶尔让出CPU
    if (++poll_count > YIELD_THRESHOLD) {
        sched_yield();
        poll_count = 0;
    }
}

// 或使用ibv_get_cq_event（节能场景）
struct ibv_comp_channel *channel = ibv_create_comp_channel(ctx);
struct ibv_cq *cq = ibv_create_cq(ctx, 100, NULL, channel, 0);
ibv_req_notify_cq(cq, 0);  // 启用中断通知

// 等待完成事件
struct ibv_cq *ev_cq;
void *ev_ctx;
ibv_get_cq_event(channel, &ev_cq, &ev_ctx);
