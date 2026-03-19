/*
 * Auto-generated from: 04_Industrial_Scenarios\03_High_Frequency_Trading\01_DPDK_Network_Stack.md
 * Line: 461
 * Language: c
 * Block ID: ab1a9620
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 未释放mbuf
void bad_process(struct rte_mbuf *m) {
    if (error_condition) {
        return;  // 内存泄漏！
    }
    // 处理...
    rte_pktmbuf_free(m);
}

// ✅ 正确释放
void good_process(struct rte_mbuf *m) {
    if (error_condition) {
        rte_pktmbuf_free(m);  // 释放
        return;
    }
    // 处理...
    rte_pktmbuf_free(m);
}
