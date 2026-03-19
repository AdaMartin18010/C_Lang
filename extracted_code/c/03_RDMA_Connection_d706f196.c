/*
 * Auto-generated from: 03_System_Technology_Domains\03_RDMA_Connection.md
 * Line: 514
 * Language: c
 * Block ID: d706f196
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 小数据直接内联，避免DMA开销
if (length <= MAX_INLINE_DATA) {
    wr.send_flags |= IBV_SEND_INLINE;  // 数据嵌入WR
}
