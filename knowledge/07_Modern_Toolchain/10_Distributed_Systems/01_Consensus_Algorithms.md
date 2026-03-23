---

## 🔗 文档关联

### 核心关联
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [内存管理](../../../01_Core_Knowledge_System/02_Core_Layer/02_Memory_Management.md) | 核心关联 | 内存管理基础 |
| [指针深度](../../../01_Core_Knowledge_System/02_Core_Layer/01_Pointer_Depth.md) | 核心关联 | 指针深度基础 |
| [并发编程](../../../03_System_Technology_Domains/14_Concurrency_Parallelism/README.md) | 核心关联 | 并发编程基础 |
| [数据类型](../../../01_Core_Knowledge_System/01_Basic_Layer/02_Data_Type_System.md) | 核心关联 | 数据类型基础 |
| [数组与指针](../../../01_Core_Knowledge_System/02_Core_Layer/05_Arrays_Pointers.md) | 核心关联 | 数组与指针基础 |

### 扩展阅读
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [软件工程](../../../01_Core_Knowledge_System/05_Engineering_Layer/README.md) | 核心关联 | 软件工程基础 |
| [形式语义](../../../02_Formal_Semantics_and_Physics/README.md) | 核心关联 | 形式语义基础 |
| [系统技术](../../../03_System_Technology_Domains/README.md) | 核心关联 | 系统技术基础 |
| [工业场景](../../../04_Industrial_Scenarios/README.md) | 核心关联 | 工业场景基础 |
| [思维表征](../../../06_Thinking_Representation/README.md) | 核心关联 | 思维表征基础 |
﻿# 分布式共识算法

## 目录

- [分布式共识算法](#分布式共识算法)
  - [目录](#目录)
  - [Raft算法](#raft算法)
    - [核心概念](#核心概念)
    - [C实现](#c实现)
  - [Gossip协议](#gossip协议)
  - [一致性哈希](#一致性哈希)
  - [参考](#参考)
  - [深入理解](#深入理解)
    - [核心原理](#核心原理)
    - [实践应用](#实践应用)
    - [最佳实践](#最佳实践)

---

## Raft算法

### 核心概念

```
Raft: 为可理解性设计的共识算法

角色:
- Leader: 处理所有客户端请求
- Follower: 被动响应请求
- Candidate: 选举中的临时状态

核心问题:
1. Leader选举
2. 日志复制
3. 安全性
```

### C实现

```c
#include <stdatomic.h>

typedef enum { STATE_FOLLOWER, STATE_CANDIDATE, STATE_LEADER } node_state_t;

typedef struct {
    int term;           /* 当前任期 */
    int voted_for;      /* 投票给哪位候选人 */
    node_state_t state;

    /* 日志 */
    log_entry_t* log;
    int log_count;
    int commit_index;
    int last_applied;

    /* Leader状态 */
    int* next_index;    /* 每个跟随者的下一个日志索引 */
    int* match_index;   /* 已复制的最高索引 */

    /* 定时器 */
    int election_timeout;
    int heartbeat_interval;
} raft_node_t;

/* 请求投票 RPC */
typedef struct {
    int term;
    int candidate_id;
    int last_log_index;
    int last_log_term;
} request_vote_req_t;

typedef struct {
    int term;
    bool vote_granted;
} request_vote_resp_t;

/* 日志追加 RPC */
typedef struct {
    int term;
    int leader_id;
    int prev_log_index;
    int prev_log_term;
    log_entry_t* entries;
    int entry_count;
    int leader_commit;
} append_entries_req_t;

/* 处理请求投票 */
request_vote_resp_t handle_request_vote(raft_node_t* node,
                                         request_vote_req_t* req) {
    request_vote_resp_t resp = { node->term, false };

    if (req->term < node->term) {
        return resp;  /* 拒绝过期请求 */
    }

    if (req->term > node->term) {
        node->term = req->term;
        node->state = STATE_FOLLOWER;
        node->voted_for = -1;
    }

    /* 检查日志是否至少一样新 */
    bool log_ok = (req->last_log_term > get_last_log_term(node)) ||
                  (req->last_log_term == get_last_log_term(node) &&
                   req->last_log_index >= get_last_log_index(node));

    if ((node->voted_for == -1 || node->voted_for == req->candidate_id)
        && log_ok) {
        node->voted_for = req->candidate_id;
        resp.vote_granted = true;
    }

    return resp;
}

/* 处理日志追加 */
bool handle_append_entries(raft_node_t* node, append_entries_req_t* req) {
    if (req->term < node->term) {
        return false;
    }

    /* 重置选举定时器 */
    reset_election_timer(node);

    if (req->term > node->term) {
        node->term = req->term;
        node->state = STATE_FOLLOWER;
        node->voted_for = -1;
    }

    /* 检查日志一致性 */
    if (req->prev_log_index > 0) {
        if (req->prev_log_index > get_last_log_index(node) ||
            node->log[req->prev_log_index].term != req->prev_log_term) {
            return false;  /* 日志不匹配 */
        }
    }

    /* 追加新日志 */
    int insert_idx = req->prev_log_index + 1;
    for (int i = 0; i < req->entry_count; i++) {
        if (insert_idx + i <= get_last_log_index(node) &&
            node->log[insert_idx + i].term != req->entries[i].term) {
            /* 删除冲突条目及之后所有 */
            node->log_count = insert_idx + i;
        }
        if (insert_idx + i > get_last_log_index(node)) {
            /* 追加 */
            node->log[node->log_count++] = req->entries[i];
        }
    }

    /* 更新commit index */
    if (req->leader_commit > node->commit_index) {
        node->commit_index = min(req->leader_commit, get_last_log_index(node));
    }

    return true;
}

/* Leader发送心跳 */
void send_heartbeats(raft_node_t* node) {
    for (int i = 0; i < node->peer_count; i++) {
        append_entries_req_t req = {
            .term = node->term,
            .leader_id = node->id,
            .prev_log_index = node->next_index[i] - 1,
            .prev_log_term = node->log[node->next_index[i] - 1].term,
            .entries = NULL,
            .entry_count = 0,
            .leader_commit = node->commit_index
        };

        rpc_append_entries(node->peers[i], &req);
    }
}
```

## Gossip协议

```c
/* 反熵传播 */

typedef struct {
    char key[64];
    char value[256];
    int version;        /* 向量时钟 */
} gossip_data_t;

typedef struct {
    gossip_data_t* data;
    int count;
} gossip_digest_t;

/* 定期与随机节点交换状态 */
void gossip_round(void) {
    int peer = select_random_peer();

    /* 发送自己的摘要 */
    gossip_digest_t my_digest = get_digest();
    send_digest(peer, &my_digest);

    /* 接收对方的摘要 */
    gossip_digest_t peer_digest;
    recv_digest(peer, &peer_digest);

    /* 比较版本，请求缺失或更新的数据 */
    for (int i = 0; i < peer_digest.count; i++) {
        gossip_data_t* d = &peer_digest.data[i];
        gossip_data_t* local = find_local(d->key);

        if (!local || d->version > local->version) {
            request_data(peer, d->key);
        }
    }
}

/* 广播新数据 */
void gossip_broadcast(gossip_data_t* data) {
    data->version = increment_vector_clock();

    /* 发送给随机子集 */
    for (int i = 0; i < FANOUT; i++) {
        int peer = select_random_peer();
        send_data(peer, data);
    }
}
```

## 一致性哈希

```c
/* 一致性哈希环 */

typedef struct node {
    uint32_t hash;
    char id[64];
    struct node* next;
} ring_node_t;

typedef struct {
    ring_node_t* head;
    int virtual_nodes;  /* 每个物理节点的虚拟节点数 */
} hash_ring_t;

uint32_t hash_key(const char* key) {
    /* MurmurHash3 */
    return murmur3_32(key, strlen(key), 0);
}

void ring_add_node(hash_ring_t* ring, const char* node_id) {
    /* 添加虚拟节点 */
    for (int i = 0; i < ring->virtual_nodes; i++) {
        char vnode_key[128];
        snprintf(vnode_key, sizeof(vnode_key), "%s#%d", node_id, i);

        ring_node_t* node = malloc(sizeof(ring_node_t));
        node->hash = hash_key(vnode_key);
        strncpy(node->id, node_id, sizeof(node->id));

        /* 插入有序链表 */
        insert_sorted(ring, node);
    }
}

const char* ring_get_node(hash_ring_t* ring, const char* key) {
    uint32_t h = hash_key(key);

    /* 找到第一个hash >= h的节点 */
    ring_node_t* node = ring->head;
    while (node && node->hash < h) {
        node = node->next;
    }

    if (!node) {
        /* 回绕到第一个 */
        node = ring->head;
    }

    return node->id;
}

/* 获取key的N个副本位置 */
void ring_get_nodes(hash_ring_t* ring, const char* key,
                    const char** nodes, int n) {
    uint32_t h = hash_key(key);
    ring_node_t* node = find_first_ge(ring, h);

    for (int i = 0; i < n; i++) {
        if (!node) node = ring->head;
        nodes[i] = node->id;

        /* 跳过同一物理节点的其他虚拟节点 */
        const char* current = node->id;
        do {
            node = node->next;
        } while (node && strcmp(node->id, current) == 0);
    }
}
```

---

## 参考

- [Raft Paper](https://raft.github.io/raft.pdf)
- [Dynamo Paper](https://www.allthingsdistributed.com/files/amazon-dynamo-sosp2007.pdf)
- [Gossip Protocols](http://www.cs.cornell.edu/home/rvr/papers/flowgossip.pdf)


---

## 深入理解

### 核心原理

深入探讨技术原理和实现细节。

### 实践应用

- 应用场景1
- 应用场景2
- 应用场景3

### 最佳实践

1. 理解基础概念
2. 掌握核心机制
3. 应用到实际项目

---

> **最后更新**: 2026-03-21
> **维护者**: AI Code Review
