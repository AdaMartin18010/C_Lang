/*
 * Auto-generated from: 03_System_Technology_Domains\03_RDMA_Connection.md
 * Line: 523
 * Language: c
 * Block ID: 839143b4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 每个操作都通知 - 高CQ压力
wr.send_flags |= IBV_SEND_SIGNALED;  // 每个WR

// ✅ 周期性信号 - 批量确认
if (++send_count % SIGNAL_BATCH == 0) {
    wr.send_flags |= IBV_SEND_SIGNALED;
}

// 定期轮询CQ
if (send_count - last_polled >= SIGNAL_BATCH) {
    ibv_poll_cq(cq, BATCH_SIZE, wc);
    last_polled = send_count;
}
