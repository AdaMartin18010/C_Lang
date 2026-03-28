# 分布式共识算法深度指南

> **层级定位**: 09_Data_Structures_Algorithms / 09_Distributed_Algorithms
> **难度级别**: L5 专家级
> **应用领域**: 分布式系统、区块链、数据库
> **最后更新**: 2026-03-28

---

## 📋 本节概要

| 属性 | 内容 |
|:-----|:-----|
| **核心概念** | CAP定理、Paxos、Raft、PBFT、共识机制 |
| **前置知识** | [数据结构](../../)、[网络编程](../../15_Network_Programming/)、[并发编程](../../14_Concurrency_Parallelism/) |
| **后续延伸** | [Raft实现](../../17_Distributed_Systems/01_Raft_Consensus.md)、[区块链](../../12_Blockchain_Crypto/) |
| **横向关联** | [系统架构](../../)、[容错理论](../../) |
| **权威来源** | Paxos Made Simple, Raft Paper, PBFT Paper |

---

## 📑 目录

- [分布式共识算法深度指南](#分布式共识算法深度指南)
  - [📋 本节概要](#-本节概要)
  - [📑 目录](#-目录)
  - [🎯 分布式共识概述](#-分布式共识概述)
    - [什么是分布式共识？](#什么是分布式共识)
    - [CAP定理](#cap定理)
    - [FLP不可能结果](#flp不可能结果)
  - [🏗️ Paxos算法](#️-paxos算法)
    - [Paxos角色](#paxos角色)
    - [Paxos两阶段协议](#paxos两阶段协议)
    - [Paxos的C实现](#paxos的c实现)
  - [⚡ Raft算法](#-raft算法)
    - [Raft核心特性](#raft核心特性)
    - [Raft状态机](#raft状态机)
    - [Leader选举](#leader选举)
    - [日志复制](#日志复制)
  - [🛡️ 拜占庭容错](#️-拜占庭容错)
    - [PBFT（实用拜占庭容错）](#pbft实用拜占庭容错)
  - [📊 实战案例](#-实战案例)
    - [案例：etcd的Raft实现](#案例etcd的raft实现)
  - [🔬 深入理解](#-深入理解)
    - [共识算法选择决策树](#共识算法选择决策树)
    - [性能对比](#性能对比)
  - [✅ 质量验收清单](#-质量验收清单)
  - [📚 相关资源](#-相关资源)
    - [论文](#论文)
    - [开源实现](#开源实现)
    - [书籍](#书籍)

---

## 🎯 分布式共识概述

### 什么是分布式共识？

**分布式共识**是指分布式系统中的多个节点就某个值达成一致的过程。

```
共识问题定义：

给定：
- N个分布式节点
- 每个节点提出一个值

要求：
1. 终止性（Termination）：所有正确节点最终达成一致
2. 一致性（Agreement）：所有正确节点选择相同的值
3. 有效性（Validity）：选择的值必须来自某个节点的提议
```

### CAP定理

```
CAP定理：分布式系统不可能同时满足以下三个特性：

Consistency（一致性）
    ↓
    所有节点在同一时刻看到相同的数据

Availability（可用性）
    ↓
    每个请求都能收到响应（成功或失败）

Partition Tolerance（分区容错性）
    ↓
    系统在网络分区时仍能运行

只能三选二：
- CA: 传统单机数据库
- CP: HBase, MongoDB（默认）, etcd
- AP: Cassandra, DynamoDB, DNS
```

### FLP不可能结果

**FLP结果**：在异步系统中，即使只有一个故障节点，也不可能实现确定性共识。

```
解决方案：
1. 引入超时机制（Partial Synchrony）
2. 使用随机化算法
3. 放宽终止性要求
```

---

## 🏗️ Paxos算法

### Paxos角色

| 角色 | 职责 |
|:-----|:-----|
| **Proposer** | 提出提案（Proposal） |
| **Acceptor** | 接受或拒绝提案 |
| **Learner** | 学习已通过的决议 |

### Paxos两阶段协议

```
Phase 1: Prepare（准备阶段）

Proposer                Acceptor
   │                       │
   │  1. Prepare(n)        │
   │ ───────────────────>  │
   │                       │
   │  2. Promise(n, v)     │
   │ <───────────────────  │
   │                       │

Phase 2: Accept（接受阶段）

   │  3. Accept(n, v)      │
   │ ───────────────────>  │
   │                       │
   │  4. Accepted(n, v)    │
   │ <───────────────────  │
```

### Paxos的C实现

```c
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define MAX_NODES 5
#define PROPOSAL_ID_BITS 32

typedef struct {
    uint64_t proposal_id;
    char value[256];
} Proposal;

typedef struct {
    uint64_t promised_id;   // 已承诺的最高proposal_id
    uint64_t accepted_id;   // 已接受的proposal_id
    char accepted_value[256];
    bool has_accepted;
} AcceptorState;

typedef struct {
    int node_id;
    AcceptorState acceptor;
    int quorum_size;
} PaxosNode;

// Phase 1: Prepare
bool paxos_prepare(PaxosNode* node, uint64_t proposal_id,
                   Proposal* promised_proposal) {
    // 只承诺比已承诺更高的proposal_id
    if (proposal_id > node->acceptor.promised_id) {
        node->acceptor.promised_id = proposal_id;

        // 如果已接受过值，返回该值
        if (node->acceptor.has_accepted) {
            promised_proposal->proposal_id = node->acceptor.accepted_id;
            strncpy(promised_proposal->value, node->acceptor.accepted_value, 255);
        }
        return true;
    }
    return false;  // 拒绝低priority的proposal
}

// Phase 2: Accept
bool paxos_accept(PaxosNode* node, Proposal* proposal) {
    // 只接受不小于已承诺的proposal_id
    if (proposal->proposal_id >= node->acceptor.promised_id) {
        node->acceptor.accepted_id = proposal->proposal_id;
        strncpy(node->acceptor.accepted_value, proposal->value, 255);
        node->acceptor.has_accepted = true;
        return true;
    }
    return false;
}
```

---

## ⚡ Raft算法

### Raft核心特性

- **强Leader**：所有日志条目从Leader流向Follower
- **Leader选举**：使用随机化超时
- **成员变更**：joint consensus机制

### Raft状态机

```
Follower                Candidate              Leader
   │                       │                      │
   │  超时未收到心跳        │                      │
   │ ───────────────────>  │                      │
   │                       │                      │
   │                       │  向所有节点请求投票   │
   │                       │ ──────────────────>  │
   │                       │                      │
   │  投票                 │                      │
   │ <───────────────────  │                      │
   │                       │                      │
   │                       │  获得多数票           │
   │                       │ ──────────────────>  │
   │                       │                      │
   │                       │                      │  发送心跳/日志
   │ <────────────────────────────────────────────  │
```

### Leader选举

```c
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <stdlib.h>

typedef enum {
    FOLLOWER,
    CANDIDATE,
    LEADER
} RaftState;

typedef struct {
    int node_id;
    RaftState state;
    int current_term;
    int voted_for;
    int vote_count;

    // 超时相关
    time_t last_heartbeat;
    int election_timeout;  // 随机 150-300ms
} RaftNode;

void raft_init(RaftNode* node, int id) {
    node->node_id = id;
    node->state = FOLLOWER;
    node->current_term = 0;
    node->voted_for = -1;
    node->vote_count = 0;
    node->last_heartbeat = time(NULL);
    node->election_timeout = 150 + rand() % 150;  // 150-300ms
}

void start_election(RaftNode* node) {
    node->state = CANDIDATE;
    node->current_term++;
    node->voted_for = node->node_id;
    node->vote_count = 1;  // 给自己投票

    // 向其他节点发送RequestVote RPC
    // ...
}

void handle_vote_request(RaftNode* node, int candidate_id, int term) {
    if (term > node->current_term) {
        // 更新term，转为follower
        node->current_term = term;
        node->state = FOLLOWER;
        node->voted_for = -1;
    }

    if (term == node->current_term) {
        if (node->voted_for == -1 || node->voted_for == candidate_id) {
            // 同意投票
            node->voted_for = candidate_id;
            // 发送VoteResponse
        }
    }
}
```

### 日志复制

```c
typedef struct LogEntry {
    int term;
    int index;
    char command[256];
} LogEntry;

typedef struct {
    LogEntry entries[10000];
    int last_log_index;
    int commit_index;

    // Leader使用
    int next_index[MAX_NODES];   // 每个节点的下一个日志索引
    int match_index[MAX_NODES];  // 每个节点已复制的最高索引
} LogStore;

void append_entries(RaftNode* node, int follower_id, LogStore* log) {
    // 构造AppendEntries RPC
    int prev_log_index = log->next_index[follower_id] - 1;
    int prev_log_term = (prev_log_index > 0) ?
                        log->entries[prev_log_index].term : 0;

    // 发送RPC...
    // 如果失败（不一致），next_index--，重试
}

void handle_append_entries(RaftNode* node, int leader_term,
                           int prev_log_index, int prev_log_term,
                           LogEntry* entries, int entry_count,
                           int leader_commit) {
    if (leader_term < node->current_term) {
        // 拒绝旧的Leader
        return;
    }

    // 重置选举超时
    node->last_heartbeat = time(NULL);
    node->state = FOLLOWER;

    // 检查日志一致性
    // 如果prev_log_index处term不匹配，拒绝
    // 否则追加新日志
    // 更新commit_index
}
```

---

## 🛡️ 拜占庭容错

### PBFT（实用拜占庭容错）

```
PBFT三阶段协议：

Request → [Client]
             │
             ▼
         [Primary]
             │
    ┌────────┼────────┐
    │        │        │
    ▼        ▼        ▼
[Replica] [Replica] [Replica]
    │        │        │
    └────────┼────────┘
             │
         Pre-Prepare
             │
    ┌────────┼────────┐
    │        │        │
    ▼        ▼        ▼
         Prepare
             │
    ┌────────┼────────┐
    │        │        │
    ▼        ▼        ▼
          Commit
             │
    ┌────────┼────────┐
    │        │        │
    ▼        ▼        ▼
         [Reply]
             │
             ▼
         [Client]

容错能力：f个拜占庭节点，需要3f+1个总节点
```

---

## 📊 实战案例

### 案例：etcd的Raft实现

```
etcd架构：

┌─────────────────────────────────────┐
│           etcd Cluster              │
│  ┌─────────┐  ┌─────────┐          │
│  │ Leader  │←→│ Follower│          │
│  │  (A)    │  │  (B)    │          │
│  └────┬────┘  └────┬────┘          │
│       │            │               │
│       └────────────┘               │
│              ↑                      │
│       ┌────────────┐               │
│       │  Follower  │               │
│       │   (C)      │               │
│       └────────────┘               │
└─────────────────────────────────────┘

特性：
- 默认5节点集群，可容忍2节点故障
- 写操作需多数派确认（3/5）
- Leader选举约1秒
- 支持线性读和串行读
```

---

## 🔬 深入理解

### 共识算法选择决策树

```
需要共识？
├── 是 → 同步环境？
│   ├── 是 → Paxos/Raft
│   └── 否 → 存在拜占庭节点？
│       ├── 是 → PBFT/HotStuff
│       └── 否 → Raft（推荐）
└── 否 → 不需要共识算法
```

### 性能对比

| 算法 | 延迟 | 吞吐 | 容错 | 复杂度 |
|:-----|:----:|:----:|:----:|:------:|
| **Paxos** | 高 | 中 | 崩溃故障 | 高 |
| **Raft** | 中 | 高 | 崩溃故障 | 低 |
| **PBFT** | 高 | 低 | 拜占庭故障 | 高 |
| **HotStuff** | 中 | 中 | 拜占庭故障 | 中 |

---

## ✅ 质量验收清单

- [x] 分布式共识概述
- [x] CAP定理详解
- [x] FLP不可能结果
- [x] Paxos算法完整实现
- [x] Raft算法（Leader选举、日志复制）
- [x] 拜占庭容错（PBFT）
- [x] 实战案例（etcd）
- [x] 算法选择决策树
- [x] 性能对比分析

---

## 📚 相关资源

### 论文

- **Paxos Made Simple** - Leslie Lamport
- **In Search of an Understandable Consensus Algorithm (Raft)** - Diego Ongaro
- **Practical Byzantine Fault Tolerance** - Miguel Castro

### 开源实现

- **etcd**: <https://github.com/etcd-io/etcd>
- **Consul**: <https://github.com/hashicorp/consul>
- **Braft**: 百度Raft实现

### 书籍

- 《Designing Data-Intensive Applications》

---

> **最后更新**: 2026-03-28
> **版本**: 1.0 - 完整版
> **维护者**: C_Lang Knowledge Base Team
