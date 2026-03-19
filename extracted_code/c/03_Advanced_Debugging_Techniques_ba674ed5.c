/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\08_Debugging_Tools\03_Advanced_Debugging_Techniques.md
 * Line: 411
 * Language: c
 * Block ID: ba674ed5
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * eBPF TC（Traffic Control）程序
 *
 * 在网络数据包进入/离开网络栈时进行处理
 * 可用于：流量整形、包过滤、负载均衡、DDoS防护
 */

#include <linux/bpf.h>
#include <linux/pkt_cls.h>
#include <linux/if_ether.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_endian.h>

char _license[] SEC("license") = "GPL";

/* 简单的速率限制器 */
struct {
    __uint(type, BPF_MAP_TYPE_LRU_HASH);
    __uint(max_entries, 10000);
    __type(key, __u32);     /* IP地址 */
    __type(value, __u64);   /* 令牌桶计数器 */
} rate_limit SEC(".maps");

#define RATE_LIMIT 100      /* 每秒最多100个包 */
#define BURST_SIZE 200      /* 突发大小 */

/**
 * TC程序入口
 *
 * TC_ACT_OK    - 允许数据包通过
 * TC_ACT_SHOT  - 丢弃数据包
 * TC_ACT_REDIRECT - 重定向到另一接口
 */
SEC("tc")
int tc_rate_limit(struct __sk_buff *skb)
{
    void *data_end = (void *)(long)skb->data_end;
    void *data = (void *)(long)skb->data;

    struct ethhdr *eth = data;
    /* eBPF验证器需要检查边界 */
    if ((void *)(eth + 1) > data_end)
        return TC_ACT_OK;  /* 数据包太短，放行 */

    /* 只处理IPv4 */
    if (eth->h_proto != bpf_htons(ETH_P_IP))
        return TC_ACT_OK;

    struct iphdr *ip = (void *)(eth + 1);
    if ((void *)(ip + 1) > data_end)
        return TC_ACT_OK;

    __u32 src_ip = ip->saddr;
    __u64 now = bpf_ktime_get_ns();

    /* 检查速率限制 */
    __u64 *last = bpf_map_lookup_elem(&rate_limit, &src_ip);
    if (last) {
        /* 简单的令牌桶算法 */
        __u64 elapsed = now - *last;
        __u64 tokens = (elapsed * RATE_LIMIT) / 1000000000;

        if (tokens > BURST_SIZE) {
            tokens = BURST_SIZE;
        }

        if (tokens < 1) {
            /* 超出速率限制，丢弃 */
            return TC_ACT_SHOT;
        }

        /* 更新最后访问时间 */
        __u64 new_time = now - ((tokens - 1) * 1000000000 / RATE_LIMIT);
        bpf_map_update_elem(&rate_limit, &src_ip, &new_time, BPF_ANY);
    } else {
        /* 新IP，初始化 */
        bpf_map_update_elem(&rate_limit, &src_ip, &now, BPF_ANY);
    }

    return TC_ACT_OK;
}

/**
 * XDP程序 - 在驱动层处理数据包
 *
 * 性能最高，但功能有限
 */
SEC("xdp")
int xdp_drop_filter(struct xdp_md *ctx)
{
    void *data_end = (void *)(long)ctx->data_end;
    void *data = (void *)(long)ctx->data;

    struct ethhdr *eth = data;
    if ((void *)(eth + 1) > data_end)
        return XDP_PASS;

    /* 示例：丢弃所有UDP包 */
    if (eth->h_proto == bpf_htons(ETH_P_IP)) {
        struct iphdr *ip = (void *)(eth + 1);
        if ((void *)(ip + 1) > data_end)
            return XDP_PASS;

        if (ip->protocol == IPPROTO_UDP)
            return XDP_DROP;
    }

    return XDP_PASS;
}
