/*
 * Auto-generated from: 03_System_Technology_Domains\07_Hardware_Security\04_Zero_Trust_Embedded_Architecture.md
 * Line: 409
 * Language: c
 * Block ID: 47b7c709
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 基于eBPF的微隔离防火墙
 * 在嵌入式Linux内核中实现L7级访问控制
 */

#include <linux/bpf.h>
#include <linux/if_ether.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include "bpf_helpers.h"

/* 进程身份映射表 */
struct {
    __uint(type, BPF_MAP_TYPE_HASH);
    __uint(max_entries, 256);
    __type(key, __u32);      // PID
    __type(value, __u32);    // 安全标签
} pid_to_label SEC(".maps");

/* 访问控制策略表 */
struct policy_key {
    __u32 src_label;
    __u32 dst_label;
    __u16 dst_port;
};

struct {
    __uint(type, BPF_MAP_TYPE_HASH);
    __uint(max_entries, 1024);
    __type(key, struct policy_key);
    __type(value, __u32);    // 动作: ALLOW=0, DENY=1, AUDIT=2
} policy_table SEC(".maps");

/* 跟踪socket连接 */
SEC("sockops")
int track_connection(struct bpf_sock_ops *skops) {
    __u32 op = skops->op;

    if (op == BPF_SOCK_OPS_TCP_CONNECT_CB) {
        /* 获取源进程身份 */
        __u32 pid = bpf_get_current_pid_tgid() >> 32;
        __u32 *src_label = bpf_map_lookup_elem(&pid_to_label, &pid);
        if (!src_label) {
            /* 未标记进程 - 默认拒绝 */
            return 0;
        }

        /* 获取目标信息 */
        __u32 dst_ip = skops->remote_ip4;
        __u16 dst_port = bpf_ntohs(skops->remote_port);

        /* 查询策略 */
        struct policy_key key = {
            .src_label = *src_label,
            .dst_label = ip_to_label(dst_ip),  // IP到标签映射
            .dst_port = dst_port,
        };

        __u32 *action = bpf_map_lookup_elem(&policy_table, &key);

        if (!action || *action == 1) {  // DENY
            bpf_printk("DENY: pid=%u label=%u -> port=%u\n",
                       pid, *src_label, dst_port);
            return 0;  // 连接将被拒绝
        }

        if (*action == 2) {  // AUDIT
            bpf_printk("AUDIT: pid=%u connected to port=%u\n",
                       pid, dst_port);
        }
    }

    return 0;
}

/* LSM钩子：强制访问控制 */
SEC("lsm/socket_connect")
int BPF_PROG(zerotrust_socket_connect, struct socket *sock,
             struct sockaddr *addr, int addrlen) {
    /* 获取当前进程标签 */
    __u32 pid = bpf_get_current_pid_tgid() >> 32;
    __u32 *label = bpf_map_lookup_elem(&pid_to_label, &pid);

    if (!label) {
        /* 无标签 = 未授权进程 */
        return -EPERM;
    }

    /* 检查是否允许网络访问 */
    if (*label == LABEL_SENSORS && addr->sa_family == AF_INET) {
        /* 传感器进程只允许访问本地网络 */
        struct sockaddr_in *sin = (struct sockaddr_in *)addr;
        if ((sin->sin_addr.s_addr & 0xFF) != 10) {  // 非10.x.x.x
            bpf_printk("SENSOR process blocked from external network\n");
            return -EPERM;
        }
    }

    return 0;  // 允许
}

char _license[] SEC("license") = "GPL";
