/*
 * Auto-generated from: 04_Industrial_Scenarios\03_High_Frequency_Trading\03_Kernel_Bypass.md
 * Line: 116
 * Language: c
 * Block ID: 0378046b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// DPDK简单接收示例
static inline void process_packet(struct rte_mbuf* mbuf) {
    struct rte_ether_hdr* eth_hdr = rte_pktmbuf_mtod(mbuf, struct rte_ether_hdr*);

    // 快速解析
    uint16_t eth_type = rte_be_to_cpu_16(eth_hdr->ether_type);

    if (eth_type == RTE_ETHER_TYPE_IPV4) {
        // 处理IPv4包
        struct rte_ipv4_hdr* ip_hdr = (struct rte_ipv4_hdr*)(eth_hdr + 1);

        if (ip_hdr->next_proto_id == IPPROTO_UDP) {
            // 处理UDP
            struct rte_udp_hdr* udp_hdr = (struct rte_udp_hdr*)((char*)ip_hdr +
                ((ip_hdr->version_ihl & 0x0F) * 4));

            uint16_t dst_port = rte_be_to_cpu_16(udp_hdr->dst_port);

            // 高频交易逻辑在这里
            if (dst_port == HTF_TRADING_PORT) {
                process_trading_packet(udp_hdr + 1);
            }
        }
    }

    rte_pktmbuf_free(mbuf);
}
