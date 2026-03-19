/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\20_Industrial_Case_Studies\02_Linux_on_RISCV.md
 * Line: 842
 * Language: c
 * Block ID: a2eb6406
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 驱动优化

// 1. 增加NAPI预算
static int budget = 300;
module_param(budget, int, 0644);

// 2. 多队列支持
static int my_poll(struct napi_struct *napi, int weight) {
    struct my_queue *q = container_of(napi, struct my_queue, napi);
    int work_done = 0;

    while (work_done < weight) {
        struct sk_buff *skb = my_rx_ring_get(q);
        if (!skb)
            break;

        napi_gro_receive(napi, skb);  // 使用GRO
        work_done++;
    }

    if (work_done < weight) {
        napi_complete_done(napi, work_done);
        my_enable_irq(q);
    }

    return work_done;
}

// 3. XDP加速
int my_xdp_prog(struct xdp_md *ctx) {
    void *data_end = (void *)(long)ctx->data_end;
    void *data = (void *)(long)ctx->data;
    struct ethhdr *eth = data;

    if (eth + 1 > data_end)
        return XDP_DROP;

    // 快速路径：直接转发
    if (eth->h_proto == htons(ETH_P_IP)) {
        return XDP_TX;  // 绕过协议栈
    }

    return XDP_PASS;
}
