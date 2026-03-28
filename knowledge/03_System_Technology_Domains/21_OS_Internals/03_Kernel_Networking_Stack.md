# 内核网络栈

> **层级**: L5 (原理层)
> **目标**: 深入理解 Linux 网络子系统架构、数据流路径、性能优化机制

---

## 目录

1. [协议分层架构](#1-协议分层架构)
2. [网络数据路径](#2-网络数据路径)
3. [拥塞控制机制](#3-拥塞控制机制)
4. [高级网络特性](#4-高级网络特性)
5. [内核实现机制](#5-内核实现机制)
6. [深入理解](#6-深入理解)
7. [思维表征](#7-思维表征)
8. [层次映射与关联](#8-层次映射与关联)

---

## 1. 协议分层架构

### 1.1 内核网络栈分层模型

```
┌─────────────────────────────────────────────────────────────┐
│                      用户空间 (User Space)                    │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────────────┐  │
│  │  应用层程序  │  │  系统调用   │  │    socket API       │  │
│  └──────┬──────┘  └──────┬──────┘  └──────────┬──────────┘  │
└─────────┼────────────────┼────────────────────┼─────────────┘
          │                │                    │
          ▼                ▼                    ▼
┌─────────────────────────────────────────────────────────────┐
│                      内核空间 (Kernel Space)                  │
│  ┌─────────────────────────────────────────────────────────┐ │
│  │  Socket 层 (BSD Socket)                                  │ │
│  │  • sock 结构体管理  • 协议族抽象  • 系统调用接口          │ │
│  └─────────────────────────────────────────────────────────┘ │
│                              │                               │
│  ┌───────────────────────────┼─────────────────────────────┐ │
│  │        INET 协议族         │                             │ │
│  │  ┌─────────────────┐      │      ┌─────────────────┐   │ │
│  │  │   TCP 协议层    │      │      │   UDP 协议层    │   │ │
│  │  │ • 连接管理      │      │      │ • 无连接传输    │   │ │
│  │  │ • 流量控制      │      │      │ • 校验和计算    │   │ │
│  │  │ • 拥塞控制      │      │      │ • 简单排队      │   │ │
│  │  └─────────────────┘      │      └─────────────────┘   │ │
│  └───────────────────────────┼─────────────────────────────┘ │
│                              │                               │
│  ┌───────────────────────────┴─────────────────────────────┐ │
│  │                   IP 协议层                              │ │
│  │  • 路由查找  • 分片重组  • 组播处理  • Netfilter 框架    │ │
│  └─────────────────────────────────────────────────────────┘ │
│                              │                               │
│  ┌───────────────────────────┴─────────────────────────────┐ │
│  │                  链路层 (L2)                             │ │
│  │  • 邻居子系统 (ARP/NDP)  • 设备驱动接口  • qdisc 排队    │ │
│  └─────────────────────────────────────────────────────────┘ │
└──────────────────────────────┬──────────────────────────────┘
                               │
┌──────────────────────────────┼──────────────────────────────┐
│                      硬件层 (Hardware)                        │
│  ┌───────────────────────────┴─────────────────────────────┐ │
│  │                   网卡设备 (NIC)                         │ │
│  │  • DMA 引擎  • Ring Buffer  • 硬件卸载 (TSO/GSO/LSO)    │ │
│  └─────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────┘
```

### 1.2 Socket 层核心数据结构

```c
// include/linux/net.h
struct socket {
    socket_state      state;          // 套接字状态
    short             type;           // SOCK_STREAM / SOCK_DGRAM
    unsigned long     flags;
    struct file      *file;           // 关联的文件对象
    struct sock      *sk;             // 指向协议特定 sock 结构
    const struct proto_ops *ops;      // 协议操作函数表
};

// include/net/sock.h
struct sock {
    // 协议成员
    unsigned short      sk_family;      // AF_INET / AF_INET6
    volatile unsigned char sk_state;    // TCP 状态
    unsigned int        sk_flags;

    // 发送/接收缓冲
    struct sk_buff_head sk_receive_queue;   // 接收队列
    struct sk_buff_head sk_write_queue;     // 发送队列
    int                 sk_rcvbuf;          // 接收缓冲区大小
    int                 sk_sndbuf;          // 发送缓冲区大小

    // 拥塞控制
    u32                 sk_pacing_rate;     // 调速速率
    struct sk_buff_head sk_tcp_rtx_queue;   // TCP 重传队列

    // 回调函数
    void (*sk_data_ready)(struct sock *sk);
    void (*sk_write_space)(struct sock *sk);
};
```

### 1.3 TCP 协议层实现

```c
// include/linux/tcp.h
struct tcp_sock {
    // inet_connection_sock 必须为首成员
    struct inet_connection_sock inet_conn;

    // 序列号管理
    __u32   snd_nxt;        // 下一个发送序号
    __u32   snd_una;        // 未确认序号
    __u32   rcv_nxt;        // 下一个期望接收序号
    __u32   rcv_wup;        // 接收窗口更新

    // 拥塞控制
    u32     snd_cwnd;       // 拥塞窗口
    u32     snd_ssthresh;   // 慢启动阈值
    u32     snd_cwnd_cnt;   // 拥塞窗口计数器

    // BBR 特定
    u32     bbr_pacing_gain;
    u32     bbr_cwnd_gain;
    u32     bbr_bw;         // 估计带宽

    // SACK 相关
    struct tcp_sack_block recv_sack_cache[4];

    // 延迟 ACK
    u32     rcv_tstamp;     // 最后接收时间戳
    u8      quick;          // 快速 ACK 模式
};
```

### 1.4 IP 层处理流程

```c
// IP 层主要处理函数

// 发送路径: 构造 IP 包头并选择路由
int ip_queue_xmit(struct sock *sk, struct sk_buff *skb) {
    struct inet_sock *inet = inet_sk(sk);
    struct net *net = sock_net(sk);
    struct ip_options_rcu *inet_opt;
    struct flowi4 *fl4;
    struct rtable *rt;
    struct iphdr *iph;

    // 获取路由缓存
    rt = skb_rtable(skb);
    if (!rt) {
        // 执行路由查找
        fl4 = &inet->cork.fl.u.ip4;
        rt = ip_route_output_flow(net, fl4, sk);
    }

    // 构造 IP 包头
    skb_push(skb, sizeof(struct iphdr));
    skb_reset_network_header(skb);
    iph = ip_hdr(skb);

    iph->version = 4;
    iph->ihl = 5;
    iph->tos = inet->tos;
    iph->tot_len = htons(skb->len);
    iph->frag_off = 0;
    iph->ttl = ip_select_ttl(inet, &rt->dst);
    iph->daddr = fl4->daddr;
    iph->saddr = fl4->saddr;
    iph->protocol = sk->sk_protocol;
    iph->id = 0;

    // 发送给下一层
    return ip_local_out(net, sk, skb);
}

// 接收路径: IP 包处理入口
int ip_rcv(struct sk_buff *skb, struct net_device *dev,
           struct packet_type *pt, struct net_device *orig_dev) {
    struct net *net = dev_net(dev);
    struct iphdr *iph;
    u32 len;

    // 包过滤 (Netfilter PREROUTING)
    if (NF_HOOK(NFPROTO_IPV4, NF_INET_PRE_ROUTING,
                net, NULL, skb, dev, NULL, ip_rcv_finish) != 1)
        return NET_RX_DROP;

    return NET_RX_SUCCESS;
}
```

---

## 2. 网络数据路径

### 2.1 sk_buff 结构详解

`sk_buff` 是 Linux 内核网络栈的核心数据结构，用于表示网络包：

```c
// include/linux/skbuff.h
struct sk_buff {
    // 链表管理
    struct sk_buff      *next;
    struct sk_buff      *prev;

    // 时间戳
    ktime_t             tstamp;

    // 网络设备
    struct net_device   *dev;

    // 控制块: 协议层私有数据
    union {
        struct tcphdr   *th;
        struct udphdr   *uh;
        struct icmphdr  *icmph;
        struct iphdr    *ipiph;
        struct ipv6hdr  *ipv6h;
        unsigned char   *raw;
    } head;

    // 数据指针
    unsigned char       *head;      // 缓冲区起始
    unsigned char       *data;      // 数据起始
    unsigned char       *tail;      // 数据结束
    unsigned char       *end;       // 缓冲区结束

    unsigned int        len, data_len;
    __u16               mac_len, hdr_len;

    // 校验和
    __wsum              csum;
    __u32               priority;

    // 协议信息
    __be16              protocol;
    __u16               transport_header;
    __u16               network_header;
    __u16               mac_header;

    // 路由与 socket
    struct dst_entry    *dst;
    struct sock         *sk;

    // 引用计数
    refcount_t          users;

    // 标志位
    __u8                pkt_type:3;
    __u8                ip_summed:2;
    __u8                cloned:1;
};
```

**sk_buff 内存布局**:

```
┌─────────────────────────────────────────────────────────────┐
│                        skb 缓冲区                            │
├─────────┬─────────┬──────────────────────────┬──────────────┤
│ Headroom│ Headers │        Payload           │   Tailroom   │
│ (16B)   │ (60B)   │        (1460B)           │   (16B)      │
├─────────┴─────────┴──────────────────────────┴──────────────┤
│                                                             │
▼ head        data                    tail                    end ▼
```

**常用操作函数**:

```c
// 分配 sk_buff
struct sk_buff *alloc_skb(unsigned int size, gfp_t priority);
struct sk_buff *dev_alloc_skb(unsigned int length);

// 数据指针操作
static inline unsigned char *skb_put(struct sk_buff *skb, unsigned int len);
static inline unsigned char *skb_push(struct sk_buff *skb, unsigned int len);
static inline unsigned char *skb_pull(struct sk_buff *skb, unsigned int len);
static inline void skb_reserve(struct sk_buff *skb, int len);

// 示例: 构造 TCP 包
struct sk_buff *build_tcp_packet(struct sock *sk) {
    struct sk_buff *skb;
    struct tcphdr *th;

    // 分配缓冲区 (头部预留 + 数据)
    skb = alloc_skb(MAX_TCP_HEADER + len, GFP_ATOMIC);
    if (!skb)
        return NULL;

    // 预留头部空间
    skb_reserve(skb, MAX_TCP_HEADER);

    // 复制应用数据
    memcpy(skb_put(skb, len), data, len);

    // 添加 TCP 头
    th = (struct tcphdr *)skb_push(skb, sizeof(struct tcphdr));
    th->source = inet_sk(sk)->inet_sport;
    th->dest = inet_sk(sk)->inet_dport;
    th->seq = htonl(tcp_sk(sk)->snd_nxt);
    th->ack_seq = htonl(tcp_sk(sk)->rcv_nxt);

    return skb;
}
```

### 2.2 发送路径 (Transmit Path)

```
┌──────────────────────────────────────────────────────────────────┐
│                         发送数据流                                │
└──────────────────────────────────────────────────────────────────┘

应用程序
    │
    ▼ write()
┌──────────────┐
│ 系统调用入口  │ sock_write_iter()
└──────┬───────┘
       │
       ▼
┌──────────────┐
│ Socket 层    │ sock_sendmsg() → inet_sendmsg()
└──────┬───────┘
       │
       ▼
┌──────────────┐
│ TCP 协议层   │ tcp_sendmsg()
│              │ • 检查窗口
│              │ • 数据分段
│              │ • 构造 sk_buff
└──────┬───────┘
       │
       ▼
┌──────────────┐
│ TCP 传输控制 │ tcp_push()
│              │ • Nagle 算法
│              │ • 拥塞窗口检查
└──────┬───────┘
       │
       ▼
┌──────────────┐
│ IP 层        │ ip_queue_xmit()
│              │ • 路由查找
│              │ • 构造 IP 头
│              │ • Netfilter (LOCAL_OUT)
└──────┬───────┘
       │
       ▼
┌──────────────┐
│ 邻居子系统   │ neigh_resolve_output()
│              │ • ARP 解析
│              │ • 构造 MAC 头
└──────┬───────┘
       │
       ▼
┌──────────────┐
│ 队列规则     │ dev_queue_xmit()
│ (qdisc)      │ • 流量整形
│              │ • 调度算法
└──────┬───────┘
       │
       ▼
┌──────────────┐
│ 网卡驱动     │ ndo_start_xmit()
│              │ • DMA 映射
│              │ • 入 Ring
└──────┬───────┘
       │
       ▼
    物理网络
```

**发送路径关键代码**:

```c
// TCP 发送主函数
int tcp_sendmsg(struct sock *sk, struct msghdr *msg, size_t size) {
    struct tcp_sock *tp = tcp_sk(sk);
    struct sk_buff *skb;
    int copied = 0;
    int err;

    // 加锁
    lock_sock(sk);

    // 检查连接状态
    if (sk->sk_state == TCP_ESTABLISHED ||
        sk->sk_state == TCP_CLOSE_WAIT) {
        goto write;  // 快速路径
    }

    // 慢速路径: 处理连接建立
    err = tcp_sendmsg_slow(sk, msg, size);
    if (err)
        goto out;

write:
    // 主发送循环
    while (msg_data_left(msg)) {
        int copy = 0;

        // 获取或分配 skb
        skb = tcp_write_queue_tail(sk);
        if (skb) {
            // 尝试追加到现有 skb
            copy = min_t(int, msg_data_left(msg),
                        skb_availroom(skb));
        }

        if (copy <= 0) {
            // 分配新 skb
new_segment:
            skb = sk_stream_alloc_skb(sk, select_size(sk),
                                      sk->sk_allocation);
            if (!skb)
                goto wait_for_space;

            skb_entail(sk, skb);
            copy = min_t(int, msg_data_left(msg),
                        skb->len);
        }

        // 复制用户数据
        err = skb_add_data_nocache(sk, skb, &msg->msg_iter, copy);
        if (err)
            goto do_error;

        copied += copy;
    }

    // 尝试立即发送
    tcp_push(sk, msg->msg_flags, size, tcp_sk(sk)->nonagle);

out:
    release_sock(sk);
    return copied;
}
```

### 2.3 接收路径与 NAPI 轮询

```
┌──────────────────────────────────────────────────────────────────┐
│                         接收数据流                                │
└──────────────────────────────────────────────────────────────────┘

    物理网络
       │
       ▼
┌──────────────┐
│ 网卡硬件     │ • DMA 到 Ring Buffer
│              │ • 硬中断通知
└──────┬───────┘
       │
       ▼
┌──────────────┐
│ 硬中断处理   │ irq_handler()
│              │ • 关闭硬中断 (NAPI)
│              │ • 触发软中断
└──────┬───────┘
       │
       ▼
┌──────────────┐
│ 软中断处理   │ net_rx_action()
│ (ksoftirqd)  │ • NAPI 轮询
│              │ • 预算限制
└──────┬───────┘
       │
       ▼
┌──────────────┐
│ 驱动轮询     │ napi_poll()
│              │ • 批量处理
│              │ • 构造成 sk_buff
└──────┬───────┘
       │
       ▼
┌──────────────┐
│ 链路层       │ eth_type_trans()
│              │ • 协议识别
│              │ • 分发处理
└──────┬───────┘
       │
       ▼
┌──────────────┐
│ IP 层        │ ip_rcv()
│              │ • 校验和验证
│              │ • 分片重组
│              │ • Netfilter (PRE_ROUTING)
└──────┬───────┘
       │
       ▼
┌──────────────┐
│ TCP 协议层   │ tcp_v4_rcv()
│              │ • 查找 socket
│              │ • 序列号检查
│              │ • 状态机处理
└──────┬───────┘
       │
       ▼
┌──────────────┐
│ Socket 层    │ sock_queue_rcv_skb()
│              │ • 放入接收队列
│              │ • 唤醒等待进程
└──────┬───────┘
       │
       ▼
    read()
  应用程序
```

**NAPI 轮询机制**:

```c
// NAPI 结构
struct napi_struct {
    struct list_head    poll_list;      // 轮询链表
    unsigned long       state;          // NAPI_STATE_SCHED
    int                 weight;         // 处理预算
    unsigned int        gro_count;
    int                 (*poll)(struct napi_struct *, int);  // 轮询函数
    struct net_device   *dev;
    struct list_head    dev_list;
};

// 软中断中的 NAPI 处理
static __latent_entropy void net_rx_action(struct softirq_action *h) {
    struct list_head *list = &sd->poll_list;
    unsigned long time_limit = jiffies + 2;
    int budget = netdev_budget;

    while (!list_empty(list)) {
        struct napi_struct *n;
        int work, weight;

        n = list_first_entry(list, struct napi_struct, poll_list);
        weight = n->weight;

        // 执行驱动轮询函数
        work = n->poll(n, min_t(int, budget, weight));

        budget -= work;

        // 检查是否超时或超预算
        if (unlikely(time_after_eq(jiffies, time_limit))) {
            sd->time_squeeze++;
            break;
        }
    }
}

// 驱动中的 NAPI 轮询示例
static int mynic_poll(struct napi_struct *napi, int budget) {
    struct mynic_ring *ring = container_of(napi, struct mynic_ring, napi);
    int done = 0;

    while (done < budget) {
        struct mynic_desc *desc = &ring->desc[ring->next_to_clean];

        // 检查描述符是否就绪
        if (!(desc->status & RX_STATUS_DD))
            break;

        // 构造成 sk_buff
        skb = build_skb(desc->addr, desc->len);

        // 提交给网络栈
        napi_gro_receive(napi, skb);

        ring->next_to_clean++;
        done++;
    }

    // 如果预算用完还有数据，继续调度
    if (done == budget)
        return budget;

    // 重新启用硬中断
    napi_complete_done(napi, done);
    enable_irq(ring->irq);

    return done;
}
```

### 2.4 软中断处理机制

```c
// 网络相关的软中断类型
enum {
    HI_SOFTIRQ = 0,         // 高优先级
    TIMER_SOFTIRQ,          // 定时器
    NET_TX_SOFTIRQ,         // 网络发送
    NET_RX_SOFTIRQ,         // 网络接收
    BLOCK_SOFTIRQ,          // 块设备
    IRQ_POLL_SOFTIRQ,       // IRQ 轮询
    TASKLET_SOFTIRQ,        // Tasklet
    SCHED_SOFTIRQ,          // 调度
    HRTIMER_SOFTIRQ,        // 高精度定时器
    RCU_SOFTIRQ,            // RCU
    NR_SOFTIRQS
};

// 网络发送软中断处理
static __latent_entropy void net_tx_action(struct softirq_action *h) {
    struct softnet_data *sd = this_cpu_ptr(&softnet_data);

    // 处理完成队列
    if (sd->completion_queue) {
        struct sk_buff *clist = sd->completion_queue;
        sd->completion_queue = NULL;

        while (clist) {
            struct sk_buff *skb = clist;
            clist = clist->next;

            // 释放 skb
            dev_kfree_skb_irq(skb);
        }
    }
}

// 触发网络软中断
static inline void ____napi_schedule(struct softnet_data *sd,
                                     struct napi_struct *napi) {
    list_add_tail(&napi->poll_list, &sd->poll_list);
    __raise_softirq_irqoff(NET_RX_SOFTIRQ);
}
```

---

## 3. 拥塞控制机制

### 3.1 拥塞控制算法架构

```c
// include/net/tcp.h
struct tcp_congestion_ops {
    struct list_head    list;
    u32                 key;
    u32                 flags;

    // 初始化/清理
    void (*init)(struct sock *sk);
    void (*release)(struct sock *sk);

    // 拥塞窗口回调
    u32 (*ssthresh)(struct sock *sk);           // 获取慢启动阈值
    void (*cong_avoid)(struct sock *sk, u32 ack, u32 acked);  // 拥塞避免
    void (*set_state)(struct sock *sk, u8 new_state);          // 状态变更
    void (*cwnd_event)(struct sock *sk, enum tcp_ca_event ev); // 窗口事件

    // 新 ACK 处理
    void (*pkts_acked)(struct sock *sk, const struct ack_sample *sample);

    // 丢包检测
    void (*undo_cwnd)(struct sock *sk);
    u32 (*min_cwnd)(const struct sock *sk);

    // RTT 采样
    void (*rtt_sample)(struct sock *sk, u32 rtt_us);

    const char  name[TCP_CA_NAME_MAX];
    struct module   *owner;
};

// 注册拥塞控制算法
int tcp_register_congestion_control(struct tcp_congestion_ops *ca);
void tcp_unregister_congestion_control(struct tcp_congestion_ops *ca);
```

### 3.2 CUBIC 算法实现

CUBIC 是 Linux 默认的 TCP 拥塞控制算法：

```c
// net/ipv4/tcp_cubic.c

/* CUBIC 参数 */
static int fast_convergence = 1;        // 快速收敛
static int beta = 717;                  // 乘性减少因子 (0.7 * 1024)
static int initial_ssthresh;            // 初始阈值
static int bic_scale = 41;              // 缩放因子
static int tcp_cubic_low_util = 0;      // 低利用率阈值

/* CUBIC 计算: W(t) = C*(t-K)^3 + W_max */
static inline void bictcp_update(struct bictcp *ca, u32 cwnd, u32 acked) {
    u32 delta, bic_target, max_cnt;
    u64 offs, t;

    ca->ack_cnt += acked;   // 记录 ACK 数量

    // 如果 cwnd 小于 last_max_cwnd，计算新的 K
    if (ca->last_max_cwnd <= cwnd) {
        ca->bic_K = 0;
        ca->bic_origin_point = cwnd;
    } else {
        // K = cubic_root((W_max - cwnd) / C)
        ca->bic_K = cubic_root((ca->last_max_cwnd - cwnd) * bic_scale / CUBIC_C_FACTOR);
        ca->bic_origin_point = ca->last_max_cwnd;
    }
}

/* 每个 ACK 调用 */
static void bictcp_cong_avoid(struct sock *sk, u32 ack, u32 acked) {
    struct tcp_sock *tp = tcp_sk(sk);
    struct bictcp *ca = inet_csk_ca(sk);

    // 慢启动阶段
    if (tp->snd_cwnd <= tp->snd_ssthresh) {
        // 指数增长
        if (tcp_slow_start(tp, acked))
            return;
    }

    // 拥塞避免阶段
    bictcp_update(ca, tp->snd_cwnd, acked);

    // 计算目标窗口
    t = (s32)(tcp_jiffies32 - ca->epoch_start) / HZ;
    if (t < ca->bic_K) {
        offs = ca->bic_K - t;
        delta = ca->bic_origin_point - bic_scale * offs * offs * offs / CUBIC_C_FACTOR;
    } else {
        offs = t - ca->bic_K;
        delta = ca->bic_origin_point + bic_scale * offs * offs * offs / CUBIC_C_FACTOR;
    }

    // 确保目标窗口 >= cwnd
    if (bic_target > cwnd)
        ca->cnt = cwnd / (bic_target - cwnd);
    else
        ca->cnt = 100 * cwnd;
}

/* 丢包处理 */
static void bictcp_cwnd_event(struct sock *sk, enum tcp_ca_event event) {
    struct tcp_sock *tp = tcp_sk(sk);
    struct bictcp *ca = inet_csk_ca(sk);

    if (event == CA_EVENT_LOSS) {
        // 保存当前最大窗口
        ca->last_max_cwnd = tp->snd_cwnd;

        // 快速收敛
        if (ca->last_max_cwnd < ca->last_cwnd && fast_convergence)
            ca->last_max_cwnd = (ca->last_max_cwnd + ca->last_cwnd) / 2;

        ca->last_cwnd = tp->snd_cwnd;
        ca->loss_cwnd = tp->snd_cwnd;

        // 乘性减少: cwnd = cwnd * beta
        tp->snd_cwnd = max(tp->snd_cwnd * beta / 1024, 2U);
    }
}

static struct tcp_congestion_ops cubictcp __read_mostly = {
    .init       = bictcp_init,
    .ssthresh   = bictcp_recalc_ssthresh,
    .cong_avoid = bictcp_cong_avoid,
    .set_state  = bictcp_state,
    .undo_cwnd  = bictcp_undo_cwnd,
    .cwnd_event = bictcp_cwnd_event,
    .pkts_acked = bictcp_acked,
    .owner      = THIS_MODULE,
    .name       = "cubic",
};
```

### 3.3 BBR 算法原理

BBR (Bottleneck Bandwidth and RTT) 是一种基于模型的拥塞控制算法：

```c
// BBR 核心状态机
enum bbr_mode {
    BBR_STARTUP,        // 启动阶段
    BBR_DRAIN,          // 排空阶段
    BBR_PROBE_BW,       // 带宽探测
    BBR_PROBE_RTT,      // RTT 探测
};

/* BBR 核心计算 */
struct bbr {
    u32 min_rtt_us;         // 最小 RTT
    u32 min_rtt_stamp;      // 最小 RTT 时间戳
    u32 probe_rtt_done_stamp;

    u64 pacing_rate;        // 调速速率
    u64 cwnd_gain;          // cwnd 增益
    u64 pacing_gain;        // pacing 增益

    u32 lt_bw;              // 长期带宽估计
    u32 full_bw;            // 全带宽估计
    u8  full_bw_reached:1;  // 是否达到全带宽
    u8  filled_pipe:1;      // 管道是否填满

    enum bbr_mode mode;     // 当前模式
    u32 cycle_idx;          // 增益周期索引
};

/* 带宽估计 */
static void bbr_update_bw(struct sock *sk, const struct rate_sample *rs) {
    struct tcp_sock *tp = tcp_sk(sk);
    struct bbr *bbr = inet_csk_ca(sk);
    u64 bw;

    // 基于 delivery rate 计算带宽
    if (rs->delivered <= 0 || rs->interval_us <= 0)
        return;

    // bw = delivered / interval
    bw = div64_long((u64)rs->delivered * BW_UNIT, rs->interval_us);

    // 更新最大带宽
    if (!rs->is_app_limited || bw >= bbr_max_bw(sk)) {
        // 使用滑动窗口最大值
        minmax_running_max(&bbr->bw, bbr_bw_rtts, bbr->rtt_cnt, bw);
    }
}

/* 计算 pacing rate 和 cwnd */
static void bbr_set_pacing_rate(struct sock *sk, u32 gain) {
    struct tcp_sock *tp = tcp_sk(sk);
    struct bbr *bbr = inet_csk_ca(sk);
    u64 rate = gain * bbr_bw(sk);

    // pacing_rate = gain * max_bw
    rate = rate >> BBR_SCALE;
    rate = min_t(u64, rate, sk->sk_max_pacing_rate);

    if (bbr->pacing_rate > 0 && bbr->full_bw_reached)
        rate = max(rate, bbr->pacing_rate * bbr_min_gain / BBR_SCALE);

    bbr->pacing_rate = rate;
    sk->sk_pacing_rate = rate;
}

/* 计算目标 inflight 数据量 */
static u32 bbr_inflight(struct sock *sk, u32 bw, int gain) {
    u32 inflight;

    // inflight = bw * min_rtt * gain
    inflight = div64_u64((u64)bw * bbr_min_rtt(sk) * gain, BBR_UNIT);
    return inflight;
}

/* 设置拥塞窗口 */
static void bbr_set_cwnd(struct sock *sk, const struct rate_sample *rs,
                         u32 acked, u32 gain) {
    struct tcp_sock *tp = tcp_sk(sk);
    struct bbr *bbr = inet_csk_ca(sk);
    u32 cwnd = 0, target_cwnd = 0;

    // 基于 BDP 计算目标 cwnd
    target_cwnd = bbr_inflight(sk, bbr_bw(sk), gain);

    // 确保有足够的数据以探测带宽
    target_cwnd += bbr_extra_acked(sk);

    // 平滑调整
    if (bbr->full_bw_reached) {
        cwnd = min(target_cwnd, tp->snd_cwnd + acked);
        cwnd = max(cwnd, bbr_cwnd_min_target);
    } else {
        cwnd = target_cwnd;
    }

    tp->snd_cwnd = min(cwnd, tp->snd_cwnd_clamp);
}
```

### 3.4 ECN 显式拥塞通知

```c
/* ECN 标志定义 */
#define IPTOS_ECN_NOT_ECT   0x00    // 不支持 ECN
#define IPTOS_ECN_ECT1      0x01    // ECN 支持类型1
#define IPTOS_ECN_ECT0      0x02    // ECN 支持类型2
#define IPTOS_ECN_CE        0x03    // 拥塞经历

/* IP 头中的 ECN 位 */
// DSCP(6bits) | ECN(2bits)

/* TCP 头中的 ECN 标志 */
#define TCPHDR_ECE  0x40    // ECN-Echo
#define TCPHDR_CWR  0x80    // Congestion Window Reduced

/* ECN 协商过程 */
// 1. SYN: 客户端发送 ECE|CWR
// 2. SYN-ACK: 服务端回复 ECE (表示支持 ECN)
// 3. ACK: 客户端确认
// 4. 数据传输: 中间路由器可设置 CE 标志
// 5. 接收方: 回复 ACK 时设置 ECE 标志
// 6. 发送方: 收到 ECE 后，减少 cwnd，设置 CWR

/* ECN 处理代码 */
static void tcp_ecn_rcv(struct sock *sk, struct sk_buff *skb) {
    struct tcp_sock *tp = tcp_sk(sk);

    // 检查 IP 头中的 CE 标志
    if (INET_ECN_is_ce(TCP_SKB_CB(skb)->ip_dsfield)) {
        tp->ecn_flags |= TCP_ECN_DEMAND_CWR;
    }

    // 检查 TCP 头中的 ECE 标志
    if (tcp_hdr(skb)->ece) {
        if (tp->ecn_flags & TCP_ECN_OK) {
            // 进入拥塞响应
            tcp_enter_cwr(sk);
        }
    }
}

/* 发送方 ECN 响应 */
static void tcp_ecn_send(struct sock *sk, struct sk_buff *skb) {
    struct tcp_sock *tp = tcp_sk(sk);
    struct tcp_skb_cb *scb = TCP_SKB_CB(skb);

    // 设置 CWR 标志
    if (scb->tcp_flags & TCPHDR_SYN) {
        // ECN 握手
        if (tp->ecn_flags & TCP_ECN_OK)
            tcp_hdr(skb)->cwr = 1;
    } else if (tp->ecn_flags & TCP_ECN_DEMAND_CWR) {
        // 需要响应拥塞通知
        tcp_hdr(skb)->cwr = 1;
        tp->ecn_flags &= ~TCP_ECN_DEMAND_CWR;
    }

    // 设置 ECE 标志 (仅用于 ECN 握手)
    if (scb->tcp_flags & TCPHDR_SYN) {
        if (tp->ecn_flags & TCP_ECN_OK)
            tcp_hdr(skb)->ece = 1;
    }
}
```

---

## 4. 高级网络特性

### 4.1 XDP (eXpress Data Path)

XDP 提供内核网络栈之前的快速数据路径：

```c
/* XDP 程序类型 */
enum xdp_action {
    XDP_ABORTED = 0,        // 异常终止
    XDP_DROP,               // 丢弃包
    XDP_PASS,               // 传递给内核网络栈
    XDP_TX,                 // 从同一接口转发
    XDP_REDIRECT,           // 重定向到其他接口/CPU
};

/* XDP 元数据结构 */
struct xdp_md {
    __u32 data;             // 数据起始
    __u32 data_end;         // 数据结束
    __u32 data_meta;        // 元数据
    __u32 ingress_ifindex;  // 入口接口索引
    __u32 rx_queue_index;   // 接收队列索引
    __u32 egress_ifindex;   // 出口接口索引
};
```

**XDP 程序示例 (DDoS 防护)**:

```c
// xdp_ddos_filter.c
#include <linux/bpf.h>
#include <linux/if_ether.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_endian.h>

struct {
    __uint(type, BPF_MAP_TYPE_HASH);
    __type(key, __u32);         // IP 地址
    __type(value, __u64);       // 最后请求时间
    __uint(max_entries, 10000);
} ip_blacklist SEC(".maps");

struct {
    __uint(type, BPF_MAP_TYPE_LRU_HASH);
    __type(key, __u32);         // IP 地址
    __type(value, __u64);       // 计数器
    __uint(max_entries, 100000);
} rate_limit SEC(".maps");

SEC("xdp")
int xdp_ddos_filter(struct xdp_md *ctx) {
    void *data_end = (void *)(long)ctx->data_end;
    void *data = (void *)(long)ctx->data;

    struct ethhdr *eth = data;
    struct iphdr *ip;
    struct tcphdr *tcp;
    __u32 src_ip;
    __u64 *last_seen, *count, now;

    // 检查以太网头
    if ((void *)(eth + 1) > data_end)
        return XDP_PASS;

    if (eth->h_proto != bpf_htons(ETH_P_IP))
        return XDP_PASS;

    // 获取 IP 头
    ip = (struct iphdr *)(eth + 1);
    if ((void *)(ip + 1) > data_end)
        return XDP_PASS;

    src_ip = ip->saddr;

    // 检查黑名单
    last_seen = bpf_map_lookup_elem(&ip_blacklist, &src_ip);
    if (last_seen)
        return XDP_DROP;

    // 速率限制检查 (仅针对 TCP SYN)
    if (ip->protocol == IPPROTO_TCP) {
        tcp = (struct tcphdr *)((void *)ip + (ip->ihl * 4));
        if ((void *)(tcp + 1) > data_end)
            return XDP_PASS;

        if (tcp->syn && !tcp->ack) {
            count = bpf_map_lookup_elem(&rate_limit, &src_ip);
            now = bpf_ktime_get_ns();

            if (count) {
                // 检查是否超过阈值 (100 SYN/秒)
                if (*count > 100) {
                    // 加入黑名单
                    bpf_map_update_elem(&ip_blacklist, &src_ip, &now, BPF_ANY);
                    return XDP_DROP;
                }
                __sync_fetch_and_add(count, 1);
            } else {
                __u64 initial = 1;
                bpf_map_update_elem(&rate_limit, &src_ip, &initial, BPF_ANY);
            }
        }
    }

    return XDP_PASS;
}

char _license[] SEC("license") = "GPL";
```

**XDP 加载与验证**:

```c
// 用户空间加载程序
#include <bpf/libbpf.h>

int main() {
    struct bpf_object *obj;
    struct bpf_program *prog;
    int prog_fd, ifindex = 2;  // eth0

    // 打开并加载 BPF 对象
    obj = bpf_object__open_file("xdp_ddos_filter.o", NULL);
    bpf_object__load(obj);

    // 获取程序 fd
    prog = bpf_object__find_program_by_name(obj, "xdp_ddos_filter");
    prog_fd = bpf_program__fd(prog);

    // 附加到网络接口 (XDP 驱动模式)
    bpf_xdp_attach(ifindex, prog_fd, XDP_FLAGS_DRV_MODE, NULL);

    // 保持运行
    while (1) sleep(1);

    // 卸载
    bpf_xdp_detach(ifindex, XDP_FLAGS_DRV_MODE, NULL);
    return 0;
}
```

### 4.2 eBPF 网络程序

eBPF 可用于网络过滤、跟踪和策略实施：

```c
// socket_filter.c - 套接字过滤
#include <linux/bpf.h>
#include <linux/if_ether.h>
#include <linux/ip.h>
#include <bpf/bpf_helpers.h>

SEC("socket")
int socket_filter(struct __sk_buff *skb) {
    // 允许所有包通过
    return skb->len;
}

// sock_ops.c - 套接字操作
#include <linux/bpf.h>
#include <linux/tcp.h>
#include <bpf/bpf_helpers.h>

struct {
    __uint(type, BPF_MAP_TYPE_SOCKHASH);
    __type(key, __u32);
    __type(value, __u64);
    __uint(max_entries, 10000);
} sock_hash SEC(".maps");

SEC("sockops")
int bpf_sockops(struct bpf_sock_ops *skops) {
    __u32 op = skops->op;
    __u32 key = 0;

    switch (op) {
    case BPF_SOCK_OPS_TCP_CONNECT_CB:
    case BPF_SOCK_OPS_TCP_LISTEN_CB:
        // 将 socket 加入哈希表
        bpf_sock_hash_update(skops, &sock_hash, &key, BPF_ANY);
        break;

    case BPF_SOCK_OPS_TCP_RTT_CB:
        // RTT 变化回调
        bpf_printk("RTT changed: %u us\n", skops->rtt_min);
        break;

    case BPF_SOCK_OPS_STATE_CB:
        if (skops->args[0] == BPF_TCP_CLOSE) {
            // 连接关闭
            bpf_sock_hash_delete(&sock_hash, &key);
        }
        break;
    }

    return 0;
}

// sk_msg.c - 消息重定向
SEC("sk_msg")
int bpf_sk_msg_redir(struct sk_msg_md *msg) {
    __u32 key = 0;

    // 重定向到目标 socket
    bpf_msg_redirect_hash(msg, &sock_hash, &key, BPF_F_INGRESS);

    return SK_PASS;
}
```

### 4.3 io_uring 网络异步 I/O

io_uring 提供高性能异步网络 I/O：

```c
// io_uring 网络服务器示例
#include <liburing.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>

#define ENTRIES 4096
#define BACKLOG 512

struct conn_info {
    int fd;
    int type;  // 0=accept, 1=recv, 2=send
};

int main() {
    struct io_uring ring;
    struct io_uring_params params = {0};
    struct io_uring_cqe *cqe;
    struct sockaddr_in serv_addr;
    int sockfd, ret;

    // 初始化 io_uring，启用单 issuer 和多接收者
    params.flags = IORING_SETUP_SINGLE_ISSUER | IORING_SETUP_DEFER_TASKRUN;
    ret = io_uring_queue_init_params(ENTRIES, &ring, &params);

    // 创建监听 socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // 绑定地址
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8080);
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    listen(sockfd, BACKLOG);

    // 设置非阻塞
    fcntl(sockfd, F_SETFL, O_NONBLOCK);

    // 提交第一个 accept
    add_accept(&ring, sockfd);

    // 事件循环
    while (1) {
        io_uring_submit_and_wait(&ring, 1);

        int head, count = 0;
        io_uring_for_each_cqe(&ring, head, cqe) {
            struct conn_info *ci = io_uring_cqe_get_data(cqe);

            if (ci->type == 0) {  // accept 完成
                int clientfd = cqe->res;
                if (clientfd >= 0) {
                    fcntl(clientfd, F_SETFL, O_NONBLOCK);

                    // 提交 recv
                    add_recv(&ring, clientfd);

                    // 继续 accept
                    add_accept(&ring, sockfd);
                }
            } else if (ci->type == 1) {  // recv 完成
                int bytes = cqe->res;
                if (bytes > 0) {
                    // 处理数据并提交 send
                    add_send(&ring, ci->fd, bytes);
                } else {
                    close(ci->fd);
                }
            } else if (ci->type == 2) {  // send 完成
                // 继续 recv
                add_recv(&ring, ci->fd);
            }

            count++;
        }

        io_uring_cq_advance(&ring, count);
    }

    io_uring_queue_exit(&ring);
    return 0;
}

// 添加 accept 请求
void add_accept(struct io_uring *ring, int fd) {
    struct io_uring_sqe *sqe = io_uring_get_sqe(ring);
    struct conn_info *ci = malloc(sizeof(*ci));

    ci->fd = fd;
    ci->type = 0;

    // 使用多 shot accept (Linux 5.19+)
    io_uring_prep_multishot_accept(sqe, fd, NULL, NULL, 0);
    io_uring_sqe_set_data(sqe, ci);
}

// 添加 recv 请求
void add_recv(struct io_uring *ring, int fd) {
    struct io_uring_sqe *sqe = io_uring_get_sqe(ring);
    struct conn_info *ci = malloc(sizeof(*ci));
    void *buf = malloc(4096);

    ci->fd = fd;
    ci->type = 1;

    // 使用 recv 多 shot
    io_uring_prep_recv(sqe, fd, buf, 4096, 0);
    io_uring_sqe_set_data(sqe, ci);
    io_uring_sqe_set_flags(sqe, IOSQE_BUFFER_SELECT);
    sqe->buf_group = 0;
}
```

### 4.4 DPDK 集成

DPDK (Data Plane Development Kit) 提供用户空间网络包处理：

```c
// DPDK 基本初始化与包处理
#include <rte_eal.h>
#include <rte_ethdev.h>
#include <rte_mbuf.h>
#include <rte_malloc.h>

#define RX_RING_SIZE 1024
#define TX_RING_SIZE 1024
#define NUM_MBUFS 8191
#define MBUF_CACHE_SIZE 250
#define BURST_SIZE 32

static const struct rte_eth_conf port_conf_default = {
    .rxmode = {
        .max_lro_pkt_size = RTE_ETHER_MAX_LEN,
    },
};

int main(int argc, char *argv[]) {
    struct rte_mempool *mbuf_pool;
    uint16_t port_id = 0;

    // 初始化 EAL
    int ret = rte_eal_init(argc, argv);

    // 创建 mbuf 池
    mbuf_pool = rte_pktmbuf_pool_create("MBUF_POOL",
                                        NUM_MBUFS,
                                        MBUF_CACHE_SIZE,
                                        0,
                                        RTE_MBUF_DEFAULT_BUF_SIZE,
                                        rte_socket_id());

    // 配置网卡
    uint16_t nb_ports = rte_eth_dev_count_avail();
    for (port_id = 0; port_id < nb_ports; port_id++) {
        struct rte_eth_conf port_conf = port_conf_default;
        struct rte_eth_rxconf rxconf;
        struct rte_eth_txconf txconf;

        // 配置端口
        ret = rte_eth_dev_configure(port_id, 1, 1, &port_conf);

        // 获取默认配置
        rxconf = dev_info.default_rxconf;
        rxconf.offloads = port_conf.rxmode.offloads;

        // 设置接收队列
        ret = rte_eth_rx_queue_setup(port_id, 0, RX_RING_SIZE,
                                     rte_eth_dev_socket_id(port_id),
                                     &rxconf, mbuf_pool);

        // 设置发送队列
        txconf = dev_info.default_txconf;
        txconf.offloads = port_conf.txmode.offloads;
        ret = rte_eth_tx_queue_setup(port_id, 0, TX_RING_SIZE,
                                     rte_eth_dev_socket_id(port_id),
                                     &txconf);

        // 启动设备
        ret = rte_eth_dev_start(port_id);

        // 启用混杂模式
        rte_eth_promiscuous_enable(port_id);
    }

    // 主处理循环
    while (1) {
        struct rte_mbuf *bufs[BURST_SIZE];

        // 批量接收
        for (port_id = 0; port_id < nb_ports; port_id++) {
            uint16_t nb_rx = rte_eth_rx_burst(port_id, 0, bufs, BURST_SIZE);

            if (nb_rx == 0)
                continue;

            // 处理包
            for (int i = 0; i < nb_rx; i++) {
                struct rte_ether_hdr *eth_hdr;
                struct rte_ipv4_hdr *ip_hdr;
                struct rte_mbuf *m = bufs[i];

                eth_hdr = rte_pktmbuf_mtod(m, struct rte_ether_hdr *);

                // 检查以太网类型
                if (eth_hdr->ether_type == rte_cpu_to_be_16(RTE_ETHER_TYPE_IPV4)) {
                    ip_hdr = (struct rte_ipv4_hdr *)(eth_hdr + 1);

                    // 简单的 L3 转发逻辑
                    // ...
                }
            }

            // 批量发送 (回环或转发)
            uint16_t nb_tx = rte_eth_tx_burst(port_id ^ 1, 0, bufs, nb_rx);

            // 释放未发送的包
            if (nb_tx < nb_rx) {
                for (int i = nb_tx; i < nb_rx; i++)
                    rte_pktmbuf_free(bufs[i]);
            }
        }
    }

    // 清理
    for (port_id = 0; port_id < nb_ports; port_id++) {
        rte_eth_dev_stop(port_id);
        rte_eth_dev_close(port_id);
    }

    rte_eal_cleanup();
    return 0;
}
```

---

## 5. 内核实现机制

### 5.1 网络数据结构关系

```
┌─────────────────────────────────────────────────────────────────┐
│                    内核网络核心数据结构                          │
└─────────────────────────────────────────────────────────────────┘

  socket (BSD Socket)
       │
       │ sock 指针
       ▼
  sock (通用套接字)
       │
       ├─────────────────┬─────────────────┐
       ▼                 ▼                 ▼
  tcp_sock          udp_sock         inet_sock
  (TCP 特定)       (UDP 特定)      (INET 通用)
       │                 │                 │
       │                 │                 │
       ▼                 ▼                 ▼
  ┌─────────┐      ┌─────────┐      ┌─────────┐
  │snd_cwnd │      │  pcslen │      │ daddr   │
  │snd_ssth │      │  pcrlen │      │ saddr   │
  │snd_nxt  │      │pending  │      │ dport   │
  │rcv_nxt  │      │         │      │ sport   │
  └─────────┘      └─────────┘      └─────────┘


  sk_buff (套接字缓冲区)
       │
       ├────────────────────────────────────────────────────┐
       │                                                      │
       ▼                                                      ▼
  ┌──────────────┐                                    ┌──────────────┐
  │ 数据指针     │                                    │ 关联指针     │
  ├──────────────┤                                    ├──────────────┤
  │ head         │──► 缓冲区起始                       │ dev          │──► 设备
  │ data         │──► 数据起始                         │ dst          │──► 路由
  │ tail         │──► 数据结束                         │ sk           │──► socket
  │ end          │──► 缓冲区结束                       │ cb[48]       │──► 层私有
  └──────────────┘                                    └──────────────┘
```

### 5.2 锁机制与并发控制

```c
/* Socket 层锁 */
struct sock {
    // 套接字锁 - 保护 sock 结构
    socket_lock_t       sk_lock;

    // 接收队列锁
    spinlock_t          sk_receive_queue_lock;

    // 回调函数锁
    rwlock_t            sk_callback_lock;

    // 原子标志
    unsigned long       sk_flags;
};

/* TCP 层锁策略 */
// 1. sock_owned_by_user: 用户空间持有锁
// 2. BH (Bottom Half) 锁: 软中断上下文
// 3. 无锁快速路径

/* 发送路径加锁 */
int tcp_sendmsg(struct sock *sk, struct msghdr *msg, size_t size) {
    int ret;

    // 获取用户锁
    lock_sock(sk);

    // 执行发送逻辑
    ret = tcp_sendmsg_locked(sk, msg, size);

    // 释放锁
    release_sock(sk);
    return ret;
}

/* 接收路径并发控制 */
int tcp_rcv(struct sk_buff *skb, struct net_device *dev, ...) {
    struct sock *sk;

    // 查找目标 socket
    sk = __inet_lookup_skb(&tcp_hashinfo, skb, ...);

    // 尝试获取锁
    if (!sock_owned_by_user(sk)) {
        // 快速路径: 直接处理
        ret = tcp_rcv_established(sk, skb);
    } else {
        // 慢速路径: 加入 backlog
        sk_add_backlog(sk, skb);
    }

    return ret;
}

/* Per-CPU 变量减少竞争 */
DECLARE_PER_CPU(struct softnet_data, softnet_data);

struct softnet_data {
    struct list_head    poll_list;      // NAPI 轮询列表
    struct sk_buff_head process_queue;  // 待处理队列

    // 每个 CPU 独立的统计
    unsigned int        processed;
    unsigned int        time_squeeze;
    unsigned int        cpu_collision;

    // 软中断 budget
    int                 budget;
};
```

### 5.3 内存管理

```c
/* 套接字缓冲区分配器 */
struct sk_buff *alloc_skb(unsigned int size, gfp_t gfp_mask) {
    struct kmem_cache *cache;
    struct sk_buff *skb;

    // 根据大小选择合适的 slab 缓存
    if (size <= SKB_SMALL_HEAD) {
        cache = skbuff_small_head_cache;
    } else {
        cache = skbuff_head_cache;
    }

    // 从 slab 分配
    skb = kmem_cache_alloc_node(cache, gfp_mask & ~GFP_DMA, node);

    // 分配数据缓冲区
    size = SKB_DATA_ALIGN(size);
    size += SKB_DATA_ALIGN(sizeof(struct skb_shared_info));

    data = kmalloc_reserve(size, gfp_mask, node, &pfmemalloc);

    // 初始化 skb
    skb->head = data;
    skb->data = data;
    skb_reset_tail_pointer(skb);
    skb->end = skb->tail + size;

    return skb;
}

/* 接收缓冲区内存管理 */
static inline int sk_rmem_schedule(struct sock *sk, struct sk_buff *skb, int size) {
    int err = 0;

    // 检查是否超过 rmem_alloc
    if (atomic_read(&sk->sk_rmem_alloc) + size > sk->sk_rcvbuf) {
        // 尝试释放旧包
        err = -ENOMEM;
    }

    return err;
}

/* TCP 内存压力处理 */
void tcp_enter_memory_pressure(struct sock *sk) {
    struct net *net = sock_net(sk);

    // 设置内存压力标志
    set_bit(TCP_MEMORY_PRESSURE, &tcp_memory_pressure);

    // 减少窗口
    tcp_sk(sk)->rcv_ssthresh = min(tcp_sk(sk)->rcv_ssthresh,
                                   4U * tcp_sk(sk)->advmss);
}

/* GRO (Generic Receive Offload) 内存优化 */
struct sk_buff *gro_receive(struct list_head *head, struct sk_buff *skb) {
    struct sk_buff *p;

    // 尝试合并到现有包
    list_for_each_entry(p, head, list) {
        struct skb_shared_info *skbinfo = skb_shinfo(p);

        // 检查是否可以合并
        if (NAPI_GRO_CB(p)->count >= 8)
            continue;

        // 执行合并
        if (can_merge) {
            skbinfo->frag_list = skb;
            NAPI_GRO_CB(p)->count++;
            p->len += skb->len;
            p->data_len += skb->len;
            p->truesize += skb->truesize;
            return p;
        }
    }

    // 无法合并，加入链表
    list_add(&skb->list, head);
    return NULL;
}
```

---

## 6. 深入理解

### 6.1 TCP 状态机形式化描述

```
┌─────────────────────────────────────────────────────────────────┐
│                     TCP 有限状态机                               │
├─────────────────────────────────────────────────────────────────┤

状态集合 S = {CLOSED, LISTEN, SYN_SENT, SYN_RECEIVED,
             ESTABLISHED, FIN_WAIT_1, FIN_WAIT_2,
             CLOSE_WAIT, CLOSING, LAST_ACK, TIME_WAIT}

初始状态: CLOSED
接受状态: {CLOSED, ESTABLISHED, CLOSE_WAIT, TIME_WAIT}

状态转移函数 δ: S × Event → S × Action

┌─────────────────┬────────────────┬────────────────┬────────────────┐
│   当前状态      │     事件       │    下一状态    │     动作       │
├─────────────────┼────────────────┼────────────────┼────────────────┤
│ CLOSED          │ active open    │ SYN_SENT       │ 发送 SYN       │
│ CLOSED          │ passive open   │ LISTEN         │ 监听端口       │
│ LISTEN          │ recv SYN       │ SYN_RECEIVED   │ 发送 SYN+ACK   │
│ LISTEN          │ close          │ CLOSED         │ 停止监听       │
│ SYN_SENT        │ recv SYN+ACK   │ ESTABLISHED    │ 发送 ACK       │
│ SYN_SENT        │ timeout        │ CLOSED         │ 错误报告       │
│ SYN_RECEIVED    │ recv ACK       │ ESTABLISHED    │ 无             │
│ SYN_RECEIVED    │ timeout        │ LISTEN         │ 重置连接       │
│ ESTABLISHED     │ close          │ FIN_WAIT_1     │ 发送 FIN       │
│ ESTABLISHED     │ recv FIN       │ CLOSE_WAIT     │ 发送 ACK       │
│ FIN_WAIT_1      │ recv ACK       │ FIN_WAIT_2     │ 无             │
│ FIN_WAIT_1      │ recv FIN+ACK   │ TIME_WAIT      │ 发送 ACK       │
│ FIN_WAIT_2      │ recv FIN       │ TIME_WAIT      │ 发送 ACK       │
│ CLOSE_WAIT      │ close          │ LAST_ACK       │ 发送 FIN       │
│ CLOSING         │ recv ACK       │ TIME_WAIT      │ 无             │
│ LAST_ACK        │ recv ACK       │ CLOSED         │ 删除 TCB       │
│ TIME_WAIT       │ 2MSL timeout   │ CLOSED         │ 删除 TCB       │
└─────────────────┴────────────────┴────────────────┴────────────────┘

形式化属性:
1. 安全性: 数据仅在 ESTABLISHED 状态传输
2. 活性: 连接最终进入 CLOSED 或 ESTABLISHED 状态
3. 互斥: 同一时刻只有一个状态活跃
```

### 6.2 拥塞控制数学模型

**CUBIC 窗口函数**:

```
W(t) = C * (t - K)^3 + W_max

其中:
- W(t): 时间 t 时的拥塞窗口
- C: CUBIC 缩放常数 (0.4)
- t: 距离上次丢包的时间
- K: 无丢包时 W 增长到 W_max 所需时间
- W_max: 丢包前的最大窗口

K = cubic_root((W_max - W) / C)

增长模式:
- 当 W < W_max (凹增长): 快速恢复带宽
- 当 W > W_max (凸增长): 谨慎探测可用带宽

收敛性证明:
设网络瓶颈带宽为 B，往返时间为 RTT
稳定状态: W / RTT = B
收敛速度: O((t - K)^3)
```

**BBR 吞吐量模型**:

```
网络管道模型:
BtlBw = 瓶颈带宽 (Bottleneck Bandwidth)
RTprop = 传播往返时间 (Round-Trip Propagation Time)

管道容量 (BDP):
BDP = BtlBw * RTprop

目标 inflight:
inflight_target = 2 * BDP  (允许一个 RTT 的 ACK 延迟)

 pacing_rate 计算:
pacing_rate = pacing_gain * BtlBw

cwnd 计算:
cwnd = cwnd_gain * BDP

增益周期 (PROBE_BW 阶段):
[1.25, 0.75, 1, 1, 1, 1, 1, 1] 每 1/8 RTprop 切换
- 1.25x: 探测更高带宽
- 0.75x: 排空队列
- 1x: 稳定传输

公平性证明:
在 N 个竞争流情况下，每个流获得 BtlBw/N 带宽
收敛时间: O(log(BtlBw_max / BtlBw_min))
```

### 6.3 吞吐量与延迟分析

```
┌─────────────────────────────────────────────────────────────────┐
│               网络性能理论边界                                   │
├─────────────────────────────────────────────────────────────────┤

延迟组成:
Total_Latency = Processing_Delay + Queuing_Delay +
                Transmission_Delay + Propagation_Delay

- Processing_Delay: 路由器/主机处理时间 (~μs)
- Queuing_Delay: 队列等待时间 (关键优化点)
- Transmission_Delay = Packet_Size / Link_Bandwidth
- Propagation_Delay = Distance / Speed_of_Light

吞吐量限制:
Throughput = min(Sender_Window, Receiver_Window, Network_Capacity) / RTT

Little's Law 应用:
In_Flight_Packets = Throughput * RTT

缓冲区大小理论:
Optimal_Buffer = BDP = Bandwidth * RTT

┌─────────────────────────────────────────────────────────────────┐
│               不同算法的延迟-吞吐量权衡                          │
├─────────────────────────────────────────────────────────────────┤

算法特性对比:

┌─────────────┬─────────────┬─────────────┬─────────────────────┐
│   算法      │  吞吐量     │   延迟      │  适用场景           │
├─────────────┼─────────────┼─────────────┼─────────────────────┤
│ Reno        │    中       │   高        │ 低带宽, 稳定网络    │
│ CUBIC       │    高       │   中        │ 高带宽, 通用场景    │
│ BBR         │    极高     │   极低      │ 高带宽, 长距离      │
│ Vegas       │    中       │   低        │ 低延迟要求          │
└─────────────┴─────────────┴─────────────┴─────────────────────┘

队列占用分析:
- Reno/CUBIC: 维持高队列占用 (BDP * 1.5)
- BBR: 保持最小队列占用 (接近 0)
```

### 6.4 用户空间与内核空间接口

```c
/* 系统调用接口 */

// 套接字创建
int socket(int domain, int type, int protocol);

// 连接管理
int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
int listen(int sockfd, int backlog);
int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);

// 数据传输
ssize_t send(int sockfd, const void *buf, size_t len, int flags);
ssize_t recv(int sockfd, void *buf, size_t len, int flags);
ssize_t sendto(int sockfd, const void *buf, size_t len, int flags,
               const struct sockaddr *dest_addr, socklen_t addrlen);
ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags,
                 struct sockaddr *src_addr, socklen_t *addrlen);

// 零拷贝接口
ssize_t sendfile(int out_fd, int in_fd, off_t *offset, size_t count);

// 高级接口 (io_uring)
int io_uring_enter(unsigned int fd, unsigned int to_submit,
                   unsigned int min_complete, unsigned int flags,
                   sigset_t *sig);

/* Socket 选项调优 */

// 设置接收缓冲区
int rcvbuf = 4 * 1024 * 1024;  // 4MB
setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &rcvbuf, sizeof(rcvbuf));

// 设置发送缓冲区
int sndbuf = 4 * 1024 * 1024;
setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, &sndbuf, sizeof(sndbuf));

// 禁用 Nagle 算法 (低延迟)
int nodelay = 1;
setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, &nodelay, sizeof(nodelay));

// 启用 TCP Fast Open
int fastopen = 5;
setsockopt(sockfd, IPPROTO_TCP, TCP_FASTOPEN, &fastopen, sizeof(fastopen));

// 设置拥塞控制算法
const char *cc = "bbr";
setsockopt(sockfd, IPPROTO_TCP, TCP_CONGESTION, cc, strlen(cc) + 1);

// 启用 SO_REUSEPORT (多进程负载均衡)
int reuseport = 1;
setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &reuseport, sizeof(reuseport));

// 设置 keepalive
int keepalive = 1;
int keepidle = 60;
int keepintvl = 10;
int keepcnt = 3;
setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, &keepalive, sizeof(keepalive));
setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPIDLE, &keepidle, sizeof(keepidle));
setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPINTVL, &keepintvl, sizeof(keepintvl));
setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPCNT, &keepcnt, sizeof(keepcnt));
```

### 6.5 性能调优指南

```c
/* sysctl 内核参数调优 */

// /etc/sysctl.conf

// 启用 BBR
net.core.default_qdisc = fq
net.ipv4.tcp_congestion_control = bbr

// 增加连接跟踪表
net.netfilter.nf_conntrack_max = 1000000

// TCP 内存调优
net.ipv4.tcp_rmem = 4096 87380 536870912
net.ipv4.tcp_wmem = 4096 65536 536870912
net.core.rmem_max = 536870912
net.core.wmem_max = 536870912
net.core.netdev_max_backlog = 5000

// TCP 优化
net.ipv4.tcp_notsent_lowat = 16384
net.ipv4.tcp_mtu_probing = 1
net.ipv4.tcp_base_mss = 1024

// 软中断调优
net.core.netdev_budget = 50000
net.core.netdev_budget_usecs = 5000
net.core.dev_weight = 4096

// 禁用反向路径过滤 (多网卡场景)
net.ipv4.conf.all.rp_filter = 0
net.ipv4.conf.default.rp_filter = 0

/* IRQ 亲和性设置 */
// /proc/irq/<irq_num>/smp_affinity
// 将网卡中断绑定到特定 CPU

echo "ff" > /proc/irq/101/smp_affinity  // 绑定到 CPU 0-7

/* 网卡多队列调优 */
ethtool -L eth0 combined 8  // 启用 8 个队列
ethtool -K eth0 gro on      // 启用 GRO
ethtool -K eth0 tso on      // 启用 TSO

/* XPS (Transmit Packet Steering) */
echo f > /sys/class/net/eth0/queues/tx-0/xps_cpus
```

---

## 7. 思维表征

### 7.1 网络数据流决策树

```
┌─────────────────────────────────────────────────────────────────┐
│                   网络包处理决策树                               │
└─────────────────────────────────────────────────────────────────┘

                              数据包到达
                                  │
                    ┌─────────────┴─────────────┐
                    ▼                           ▼
              [XDP attached?]              [正常路径]
                    │                           │
          ┌─────────┴─────────┐                 ▼
          ▼                   ▼            [NAPI 轮询]
      [XDP_PASS]        [XDP_DROP]              │
          │                                   [软中断]
          ▼                                     │
    [继续处理]                      ┌───────────┴───────────┐
          │                         ▼                       ▼
          ▼                    [IP 层]                 [GRO 合并]
    [TC/iptables]                    │                       │
          │                   [路由查找]                     ▼
          ▼                         │                  [协议处理]
    [网络栈入口]          ┌─────────┴─────────┐              │
          │               ▼                   ▼              ▼
          ▼          [本地投递]          [转发路径]    [TCP/UDP层]
    [协议识别]               │                   │              │
          │                ▼                   ▼              ▼
          ▼          [传输层]          [邻居子系统]    [socket查找]
    [TCP/UDP/...]            │                   │              │
          │                ▼                   ▼              ▼
          ▼          [socket队列]        [链路层输出]   [数据交付]
    [状态机处理]             │                   │              │
          │                ▼                   ▼              ▼
          ▼          [唤醒进程]          [网卡队列]    [用户空间]
    [数据入队]
          │
          ▼
    [ACK 处理]
```

### 7.2 拥塞控制算法对比矩阵

```
┌──────────────────────────────────────────────────────────────────────────┐
│                       拥塞控制算法特性对比矩阵                            │
├──────────────┬──────────┬──────────┬──────────┬──────────┬──────────────┤
│   特性       │  Reno    │  CUBIC   │   BBR    │  Vegas   │   Westwood   │
├──────────────┼──────────┼──────────┼──────────┼──────────┼──────────────┤
│ 设计年代     │ 1990s    │ 2006     │ 2016     │ 1995     │ 2001         │
├──────────────┼──────────┼──────────┼──────────┼──────────┼──────────────┤
│ 探测方式     │ 丢包驱动 │ 丢包驱动 │ 模型驱动 │ 延迟驱动 │ 带宽估计     │
├──────────────┼──────────┼──────────┼──────────┼──────────┼──────────────┤
│ 窗口增长     │ 线性/指数│ 三次函数 │ 调速控制 │ 线性     │ 自适应       │
├──────────────┼──────────┼──────────┼──────────┼──────────┼──────────────┤
│ 队列占用     │    高    │   高     │   低     │   低     │   中         │
├──────────────┼──────────┼──────────┼──────────┼──────────┼──────────────┤
│ RTT 公平性   │   差     │   差     │   好     │   好     │   好         │
├──────────────┼──────────┼──────────┼──────────┼──────────┼──────────────┤
│ 带宽利用率   │   中     │   高     │   极高   │   中     │   高         │
├──────────────┼──────────┼──────────┼──────────┼──────────┼──────────────┤
│ 收敛速度     │   慢     │   中     │   快     │   中     │   快         │
├──────────────┼──────────┼──────────┼──────────┼──────────┼──────────────┤
│ 无线链路适应 │   差     │   中     │   好     │   差     │   好         │
├──────────────┼──────────┼──────────┼──────────┼──────────┼──────────────┤
│ 计算复杂度   │   低     │   中     │   高     │   低     │   中         │
├──────────────┼──────────┼──────────┼──────────┼──────────┼──────────────┤
│ 适用场景     │ 传统网络 │ 通用场景 │ 高带宽   │ 交互式   │ 无线网络     │
│              │          │          │ 长距离   │          │              │
├──────────────┼──────────┼──────────┼──────────┼──────────┼──────────────┤
│ Linux默认    │   否     │   是     │   否     │   否     │   否         │
├──────────────┼──────────┼──────────┼──────────┼──────────┼──────────────┤
│ 内核版本     │ 2.0+     │ 2.6.19+  │ 4.9+     │ 2.2+     │ 2.6+         │
└──────────────┴──────────┴──────────┴──────────┴──────────┴──────────────┘
```

### 7.3 内核网络栈架构图

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                           Linux 内核网络栈架构                               │
└─────────────────────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────────────────────┐
│                               用户空间层                                     │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐   │
│  │   应用程序   │  │ 系统调用层   │  │  socket API  │  │  io_uring    │   │
│  │  (curl/nginx)│  │              │  │              │  │  (async I/O) │   │
│  └──────┬───────┘  └──────┬───────┘  └──────┬───────┘  └──────┬───────┘   │
└─────────┼─────────────────┼─────────────────┼─────────────────┼───────────┘
          │                 │                 │                 │
          ▼                 ▼                 ▼                 ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│                            系统调用接口层                                    │
│  ┌───────────────────────────────────────────────────────────────────────┐  │
│  │  sys_socketcall → sys_sendto/sys_recvfrom/sys_setsockopt/...         │  │
│  └───────────────────────────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────────────────────────┘
          │
          ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│                             Socket 抽象层                                    │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐   │
│  │  BSD Socket  │  │  sock 结构   │  │  协议切换表  │  │  文件描述符  │   │
│  │              │  │              │  │  proto_ops   │  │              │   │
│  └──────┬───────┘  └──────┬───────┘  └──────┬───────┘  └──────────────┘   │
└─────────┼─────────────────┼─────────────────┼──────────────────────────────┘
          │                 │                 │
          ▼                 ▼                 ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│                              传输层 (L4)                                     │
│  ┌─────────────────────────────┐  ┌──────────────────────────────────────┐  │
│  │           TCP               │  │              UDP                     │  │
│  │  ┌─────────┐ ┌─────────┐   │  │  ┌─────────┐ ┌─────────┐            │  │
│  │  │ 连接管理 │ │ 拥塞控制 │   │  │  │ 无连接  │ │ 校验和  │            │  │
│  │  │ 状态机  │ │ CUBIC   │   │  │  │ 复用    │ │ 计算    │            │  │
│  │  │ SACK    │ │ BBR     │   │  │  │ 排队    │ │         │            │  │
│  │  │ FastO.  │ │ ECN     │   │  │  └─────────┘ └─────────┘            │  │
│  │  └─────────┘ └─────────┘   │  │                                     │  │
│  └─────────────────────────────┘  └──────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────────────────────────┘
          │
          ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│                               网络层 (L3)                                    │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐   │
│  │   IPv4/IPv6  │  │   路由子系统 │  │  Netfilter   │  │    IPSec     │   │
│  │              │  │  (FIB/缓存)  │  │  (iptable)   │  │              │   │
│  └──────┬───────┘  └──────┬───────┘  └──────┬───────┘  └──────────────┘   │
└─────────┼─────────────────┼─────────────────┼──────────────────────────────┘
          │
          ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│                               链路层 (L2)                                    │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐   │
│  │  邻居子系统  │  │   qdisc      │  │   XDP/eBPF   │  │   网桥/VLAN  │   │
│  │  (ARP/NDP)   │  │ (流量控制)   │  │  (快速路径)  │  │              │   │
│  └──────┬───────┘  └──────┬───────┘  └──────┬───────┘  └──────────────┘   │
└─────────┼─────────────────┼─────────────────┼──────────────────────────────┘
          │
          ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│                              设备驱动层                                      │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐   │
│  │   NAPI 轮询  │  │  Ring Buffer │  │    DMA       │  │  硬件卸载    │   │
│  │              │  │              │  │   引擎       │  │ TSO/GSO/UFO  │   │
│  └──────────────┘  └──────────────┘  └──────────────┘  └──────────────┘   │
└─────────────────────────────────────────────────────────────────────────────┘
          │
          ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│                               硬件层                                         │
│  ┌───────────────────────────────────────────────────────────────────────┐  │
│  │                    NIC (网络接口控制器)                               │  │
│  │  ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────────┐             │  │
│  │  │  RX队列  │  │  TX队列  │  │  MAC地址 │  │  PHY芯片 │             │  │
│  │  └──────────┘  └──────────┘  └──────────┘  └──────────┘             │  │
│  └───────────────────────────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────────────────────────┘
```

### 7.4 数据流时序图

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                      TCP 三次握手与数据传输时序                               │
└─────────────────────────────────────────────────────────────────────────────┘

客户端                                              服务端
  │                                                   │
  │────────────── SYN, seq=x ────────────────────────>│
  │              [SYN_SENT]                           │ [LISTEN]
  │                                                   │
  │<───────── SYN+ACK, seq=y, ack=x+1 ───────────────│
  │              [ESTABLISHED]                        │ [SYN_RCVD]
  │                                                   │
  │────────────── ACK, ack=y+1 ──────────────────────>│
  │                                                   │ [ESTABLISHED]
  │                                                   │
  │                   数据传输阶段                    │
  │                                                   │
  │────────────── PSH+ACK, seq=x+1 ──────────────────>│
  │  data[0:1000]                                     │
  │                                                   │
  │<───────────────── ACK, ack=x+1001 ────────────────│
  │                                                   │
  │<────────── PSH+ACK, seq=y+1, ack=x+1001 ─────────│
  │                                                   │ data[0:500]
  │                                                   │
  │───────────────── ACK, ack=y+501 ─────────────────>│
  │                                                   │
  │                   连接终止                        │
  │                                                   │
  │────────────── FIN, seq=x+1001 ───────────────────>│
  │              [FIN_WAIT_1]                         │
  │                                                   │
  │<───────────────── ACK, ack=x+1002 ────────────────│
  │              [FIN_WAIT_2]                         │
  │                                                   │
  │<────────────── FIN, seq=y+501 ────────────────────│
  │                                                   │ [LAST_ACK]
  │                                                   │
  │───────────────── ACK, ack=y+502 ─────────────────>│
  │              [TIME_WAIT]                          │ [CLOSED]
  │                                                   │
  │              2MSL 超时                            │
  │              [CLOSED]                             │
  │                                                   │
```

---

## 8. 层次映射与关联

### 8.1 与进程调度层的关联

```
┌─────────────────────────────────────────────────────────────────┐
│               网络子系统与进程调度的关联                         │
└─────────────────────────────────────────────────────────────────┘

网络事件驱动进程调度:

1. 数据到达
      │
      ▼
┌─────────────┐
│ 软中断处理  │ ──► 将数据加入 socket 接收队列
└──────┬──────┘
       │
       ▼
┌─────────────┐
│ 唤醒进程    │ ──► wake_up_interruptible(sk_sleep(sk))
└──────┬──────┘
       │
       ▼
┌─────────────┐
│ 调度器      │ ──► 将进程加入可运行队列
└──────┬──────┘
       │
       ▼
┌─────────────┐
│ 进程执行    │ ──► 读取数据
└─────────────┘

关键关联点:
- socket 等待队列 ↔ 进程 wait_queue
- 软中断上下文 ↔ 进程上下文
- 接收缓冲区满 → 流量控制 → 发送方阻塞
- 发送缓冲区空 → 调度唤醒 → 继续发送
```

### 8.2 与内存管理层的关联

```
┌─────────────────────────────────────────────────────────────────┐
│               网络子系统与内存管理的关联                         │
└─────────────────────────────────────────────────────────────────┘

sk_buff 内存分配路径:

应用程序数据
      │
      ▼
┌─────────────────┐
│ 用户空间缓冲区  │ ──► malloc/mmap
└────────┬────────┘
         │ copy_from_user
         ▼
┌─────────────────┐
│ sk_buff 数据区  │ ──► kmalloc/kmem_cache_alloc
└────────┬────────┘
         │ DMA 映射
         ▼
┌─────────────────┐
│ DMA 一致性内存  │ ──► dma_alloc_coherent
└─────────────────┘

内存压力处理:
- TCP 内存压力 → 减小接收窗口 → 减少内存占用
- 套接字缓冲区限制 → 丢包/阻塞
- 高阶内存不足 → 强制回收/压缩
```

### 8.3 与文件系统的关联

```
┌─────────────────────────────────────────────────────────────────┐
│               网络子系统与文件系统的关联                         │
└─────────────────────────────────────────────────────────────────┘

Socket 作为文件:

┌──────────────┐
│ 文件描述符   │ fd = socket(...)
└──────┬───────┘
       │
       ▼
┌──────────────┐
│ 文件结构体   │ struct file
│  f_op        │ ──► socket_file_ops
└──────┬───────┘
       │
       ▼
┌──────────────┐
│ 套接字结构   │ struct socket
└──────┬───────┘
       │
       ▼
┌──────────────┐
│ sock 结构    │ struct sock
└──────────────┘

文件操作映射:
- read/write → sock_read_iter/sock_write_iter
- poll/select → sock_poll
- ioctl → sock_ioctl
- close → sock_close

sendfile 优化 (零拷贝):
文件页缓存 ──► socket 发送队列 (无用户空间复制)
```

---

## 总结

Linux 内核网络栈是一个高度优化的分层架构，从 Socket 抽象层到硬件驱动，每一层都承担着特定的职责：

1. **协议分层**：通过清晰的抽象接口实现模块化设计，支持多种网络协议
2. **数据路径优化**：sk_buff 结构、NAPI 轮询、GRO/GSO 等机制大幅提升处理效率
3. **拥塞控制演进**：从 Reno 到 CUBIC 再到 BBR，算法不断优化以应对高带宽延迟网络
4. **现代特性**：XDP/eBPF 提供内核级可编程性，io_uring 革新异步 I/O 模型

理解这些底层机制，对于构建高性能网络应用、进行系统级调优以及排查复杂网络问题至关重要。

---

**参考资源**:

- Linux 内核源码: `net/ipv4/`, `net/core/`
- 《TCP/IP 详解》
- 《Linux 内核网络栈源代码情景分析》
- eBPF/XDP 官方文档: <https://docs.cilium.io/>
