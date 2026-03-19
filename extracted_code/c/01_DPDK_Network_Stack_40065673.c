/*
 * Auto-generated from: 04_Industrial_Scenarios\03_High_Frequency_Trading\01_DPDK_Network_Stack.md
 * Line: 234
 * Language: c
 * Block ID: 40065673
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 高性能数据包处理

#define BURST_SIZE 32

// 接收数据包
void receive_packets(uint16_t portid, uint16_t queueid) {
    struct rte_mbuf *bufs[BURST_SIZE];

    // 批量接收
    uint16_t nb_rx = rte_eth_rx_burst(portid, queueid, bufs, BURST_SIZE);

    if (nb_rx == 0)
        return;

    // 处理每个数据包
    for (uint16_t i = 0; i < nb_rx; i++) {
        struct rte_mbuf *mbuf = bufs[i];

        // 获取数据包指针
        uint8_t *pkt_data = rte_pktmbuf_mtod(mbuf, uint8_t *);
        uint16_t pkt_len = rte_pktmbuf_data_len(mbuf);

        // 解析以太网头
        struct rte_ether_hdr *eth_hdr = (struct rte_ether_hdr *)pkt_data;

        // 检查IP数据包
        if (eth_hdr->ether_type == rte_cpu_to_be_16(RTE_ETHER_TYPE_IPV4)) {
            process_ipv4_packet(mbuf);
        } else if (eth_hdr->ether_type == rte_cpu_to_be_16(RTE_ETHER_TYPE_IPV6)) {
            process_ipv6_packet(mbuf);
        }

        // 释放mbuf
        rte_pktmbuf_free(mbuf);
    }
}

// L3转发示例
void l3_forward(struct rte_mbuf *mbuf) {
    uint8_t *pkt_data = rte_pktmbuf_mtod(mbuf, uint8_t *);
    struct rte_ether_hdr *eth_hdr = (struct rte_ether_hdr *)pkt_data;
    struct rte_ipv4_hdr *ip_hdr = (struct rte_ipv4_hdr *)(eth_hdr + 1);

    // TTL递减
    if (ip_hdr->time_to_live <= 1) {
        // TTL过期，丢弃
        rte_pktmbuf_free(mbuf);
        return;
    }
    ip_hdr->time_to_live--;

    // 重新计算IP校验和（增量更新）
    ip_hdr->hdr_checksum = rte_ipv4_cksum_update(
        ip_hdr->hdr_checksum,
        rte_cpu_to_be_16(0x0100)  // TTL字段变化
    );

    // 查找路由表
    uint32_t dst_ip = rte_be_to_cpu_32(ip_hdr->dst_addr);
    struct route_entry *route = route_lookup(dst_ip);

    if (route == NULL) {
        rte_pktmbuf_free(mbuf);
        return;
    }

    // 修改MAC地址
    memcpy(eth_hdr->dst_addr.addr_bytes, route->next_hop_mac, RTE_ETHER_ADDR_LEN);
    memcpy(eth_hdr->src_addr.addr_bytes, route->src_mac, RTE_ETHER_ADDR_LEN);

    // 发送
    uint16_t nb_tx = rte_eth_tx_burst(route->portid, 0, &mbuf, 1);
    if (nb_tx == 0) {
        rte_pktmbuf_free(mbuf);
    }
}

// 批处理发送
void send_packets(uint16_t portid, uint16_t queueid,
                  struct rte_mbuf **pkts, uint16_t nb_pkts) {
    uint16_t sent = 0;

    while (sent < nb_pkts) {
        uint16_t nb_tx = rte_eth_tx_burst(portid, queueid,
                                          pkts + sent, nb_pkts - sent);
        sent += nb_tx;
    }
}
