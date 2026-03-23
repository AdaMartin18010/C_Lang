# 分布式系统 (Distributed Systems)

## 概述

分布式系统是由多个独立计算机组成的系统，这些计算机通过网络进行通信和协调，对外表现为一个统一的整体。理解分布式系统的核心概念对于构建可靠的现代软件系统至关重要。

## 目录

- [分布式系统 (Distributed Systems)](#分布式系统-distributed-systems)
  - [概述](#概述)
  - [目录](#目录)
  - [分布式共识](#分布式共识)
    - [什么是分布式共识](#什么是分布式共识)
    - [共识问题特性](#共识问题特性)
    - [典型共识算法对比](#典型共识算法对比)
    - [Raft 算法详解](#raft-算法详解)
      - [节点状态转换](#节点状态转换)
      - [领导选举流程](#领导选举流程)
      - [日志复制流程](#日志复制流程)
  - [一致性模型](#一致性模型)
    - [一致性强度谱系](#一致性强度谱系)
    - [一致性模型对比](#一致性模型对比)
    - [实现机制](#实现机制)
      - [线性一致性实现](#线性一致性实现)
      - [因果一致性实现](#因果一致性实现)
      - [最终一致性实现](#最终一致性实现)
  - [CAP 定理](#cap-定理)
    - [CAP 定理概述](#cap-定理概述)
    - [CAP 权衡选择](#cap-权衡选择)
    - [CAP 权衡的实际影响](#cap-权衡的实际影响)
      - [CP 系统行为](#cp-系统行为)
      - [AP 系统行为](#ap-系统行为)
    - [PACELC 定理](#pacelc-定理)
  - [分布式事务](#分布式事务)
    - [分布式事务概述](#分布式事务概述)
    - [两阶段提交 (2PC)](#两阶段提交-2pc)
      - [2PC 实现](#2pc-实现)
    - [三阶段提交 (3PC)](#三阶段提交-3pc)
    - [Saga 模式](#saga-模式)
    - [分布式事务方案对比](#分布式事务方案对比)
  - [总结](#总结)
  - [深入理解](#深入理解)
    - [核心原理](#核心原理)
    - [实践应用](#实践应用)
    - [最佳实践](#最佳实践)

---

## 分布式共识

### 什么是分布式共识

分布式共识是指在一个由多个节点组成的分布式系统中，所有节点就某个值达成一致的过程。这是分布式系统中最基本也是最重要的问题之一。

### 共识问题特性

| 特性 | 描述 | 重要性 |
|-----|------|-------|
| 终止性 | 所有正确节点最终必须做出决定 | 保证系统活性 |
| 一致性 | 所有正确节点必须做出相同的决定 | 保证安全性 |
| 有效性 | 决定必须是某个节点提出的值 | 保证有效性 |
| 容错性 | 系统能在部分节点故障时继续工作 | 保证可用性 |

### 典型共识算法对比

| 算法 | 容错类型 | 容错节点数 | 通信复杂度 | 适用场景 |
|-----|---------|-----------|-----------|---------|
| Paxos | 崩溃容错 | 2f+1 容忍 f | O(n²) | 理论基准 |
| Multi-Paxos | 崩溃容错 | 2f+1 容忍 f | O(n) 稳定后 | 实际生产 |
| Raft | 崩溃容错 | 2f+1 容忍 f | O(n) | 教学和工程 |
| PBFT | 拜占庭容错 | 3f+1 容忍 f | O(n²) | 区块链 |
| ZAB | 崩溃容错 | 2f+1 容忍 f | O(n) | ZooKeeper |

### Raft 算法详解

Raft 是一种易于理解的共识算法，它将共识问题分解为三个子问题：

```text
┌─────────────────────────────────────────────────────────────┐
│                      Raft 算法结构                          │
└─────────────────────────────────────────────────────────────┘
                              │
        ┌─────────────────────┼─────────────────────┐
        │                     │                     │
        ▼                     ▼                     ▼
┌───────────────┐     ┌───────────────┐     ┌───────────────┐
│  领导选举      │     │   日志复制    │     │   安全性      │
├───────────────┤     ├───────────────┤     ├───────────────┤
│ - 选举超时     │     │ - 客户端请求  │     │ - 提交规则    │
│ - 心跳机制     │     │ - 日志条目    │     │ - 状态机安全  │
│ - 任期(term)  │      │ - 一致性检查  │     │ - 领导人完整  │
└───────────────┘     └───────────────┘     └───────────────┘
```

#### 节点状态转换

```c
// Raft 节点状态定义
typedef enum {
    STATE_FOLLOWER,   // 跟随者：被动响应请求
    STATE_CANDIDATE,  // 候选者：发起选举
    STATE_LEADER      // 领导者：处理客户端请求
} raft_state_t;

typedef struct {
    // 持久状态
    uint64_t current_term;    // 当前任期
    int voted_for;            // 当前任期投票给谁
    log_entry_t* log;         // 日志条目数组
    size_t log_count;

    // 易失状态
    raft_state_t state;
    int commit_index;         // 已知提交的最高日志索引
    int last_applied;         // 已应用到状态机的最高索引

    // 领导者易失状态 (重新选举后重置)
    int* next_index;          // 每个节点的下一个日志索引
    int* match_index;         // 每个节点已复制的最高索引

    // 定时器
    struct timeval election_timeout;
    struct timeval heartbeat_interval;
} raft_node_t;
```

#### 领导选举流程

```c
// 选举超时触发
void on_election_timeout(raft_node_t* node) {
    // 1. 增加任期
    node->current_term++;

    // 2. 转换为候选者
    node->state = STATE_CANDIDATE;
    node->voted_for = node->id;

    // 3. 给自己投票
    int votes_received = 1;

    // 4. 向其他节点发送 RequestVote RPC
    for (int i = 0; i < node->cluster_size; i++) {
        if (i == node->id) continue;

        request_vote_args_t args = {
            .term = node->current_term,
            .candidate_id = node->id,
            .last_log_index = node->log_count - 1,
            .last_log_term = node->log_count > 0 ?
                node->log[node->log_count - 1].term : 0
        };

        send_request_vote(i, &args);
    }

    // 5. 重置选举定时器
    reset_election_timer(node);
}

// 处理 RequestVote RPC
request_vote_reply_t handle_request_vote(raft_node_t* node,
                                         request_vote_args_t* args) {
    request_vote_reply_t reply = { .term = node->current_term };

    // 1. 任期检查
    if (args->term < node->current_term) {
        reply.vote_granted = false;
        return reply;
    }

    // 2. 更新任期
    if (args->term > node->current_term) {
        node->current_term = args->term;
        node->voted_for = -1;
        become_follower(node);
    }

    // 3. 投票条件检查
    bool can_vote = (node->voted_for == -1 ||
                     node->voted_for == args->candidate_id);
    bool log_is_up_to_date = is_log_up_to_date(node, args);

    reply.vote_granted = can_vote && log_is_up_to_date;
    if (reply.vote_granted) {
        node->voted_for = args->candidate_id;
        reset_election_timer(node);
    }

    return reply;
}
```

#### 日志复制流程

```c
// 客户端请求处理 (仅领导者)
int raft_append_entry(raft_node_t* node, const char* data, size_t len) {
    if (node->state != STATE_LEADER) {
        return -1;  // 非领导者，重定向
    }

    // 1. 创建日志条目
    log_entry_t entry = {
        .term = node->current_term,
        .index = node->log_count,
        .data = malloc(len),
        .data_len = len
    };
    memcpy(entry.data, data, len);

    // 2. 追加到本地日志
    node->log[node->log_count++] = entry;

    // 3. 并行发送 AppendEntries RPC
    for (int i = 0; i < node->cluster_size; i++) {
        if (i == node->id) continue;
        send_append_entries(node, i);
    }

    return entry.index;
}

// 处理 AppendEntries RPC
append_entries_reply_t handle_append_entries(raft_node_t* node,
                                             append_entries_args_t* args) {
    append_entries_reply_t reply = {
        .term = node->current_term,
        .success = false
    };

    // 1. 任期检查
    if (args->term < node->current_term) {
        return reply;
    }

    // 2. 成为跟随者并重置定时器
    if (args->term > node->current_term) {
        node->current_term = args->term;
    }
    become_follower(node);
    reset_election_timer(node);

    // 3. 日志一致性检查
    if (args->prev_log_index >= 0) {
        if (args->prev_log_index >= node->log_count ||
            node->log[args->prev_log_index].term != args->prev_log_term) {
            // 日志不一致，需要回溯
            reply.conflict_index = find_conflict_index(node, args);
            return reply;
        }
    }

    // 4. 追加新条目
    int insert_index = args->prev_log_index + 1;
    for (int i = 0; i < args->entry_count; i++) {
        if (insert_index + i < node->log_count) {
            if (node->log[insert_index + i].term != args->entries[i].term) {
                // 删除冲突条目及之后的所有条目
                truncate_log(node, insert_index + i);
            }
        }
        if (insert_index + i >= node->log_count) {
            append_log_entry(node, &args->entries[i]);
        }
    }

    // 5. 更新提交索引
    if (args->leader_commit > node->commit_index) {
        node->commit_index = min(args->leader_commit, node->log_count - 1);
        apply_committed_entries(node);
    }

    reply.success = true;
    return reply;
}
```

---

## 一致性模型

### 一致性强度谱系

```text
一致性强度 (从强到弱):

线性一致性 ──► 顺序一致性 ──► 因果一致性 ──► 会话一致性 ──► 最终一致性
  (最强)                                              (最弱)
    │                      │                      │
    ▼                      ▼                      ▼
  严格串行化           所有进程看到            无因果关系的
  所有操作             相同顺序               操作可以乱序
```

### 一致性模型对比

| 一致性模型 | 描述 | 延迟 | 可用性 | 典型应用 |
|-----------|------|-----|-------|---------|
| 线性一致性 | 所有操作看起来是原子且即时发生的 | 高 | 低 | 金融交易 |
| 顺序一致性 | 所有进程看到的操作顺序一致 | 中高 | 中 | 分布式锁 |
| 因果一致性 | 有因果关系的操作顺序一致 | 中 | 中高 | 社交网络 |
| 会话一致性 | 单个会话内的读能看到之前的写 | 中 | 中高 | 购物车 |
| 最终一致性 | 无新更新时最终所有副本一致 | 低 | 高 | DNS、CDN |
| 读己之写 | 总是能看到自己的写入 | 中 | 中 | 用户配置 |

### 实现机制

#### 线性一致性实现

```c
// 基于 Paxos/Raft 的线性一致性存储
typedef struct {
    raft_node_t* raft;           // 底层共识层
    hash_table_t* data;          // 数据存储
    pthread_mutex_t apply_mutex; // 状态机互斥锁
} linear_store_t;

// 写操作 - 必须通过共识
int linear_store_write(linear_store_t* store, const char* key,
                       const void* value, size_t len) {
    // 构建写操作日志条目
    write_op_t op = {
        .type = OP_WRITE,
        .key = strdup(key),
        .value = malloc(len),
        .value_len = len
    };
    memcpy(op.value, value, len);

    // 提交到 Raft 集群
    int index = raft_append_entry(store->raft, &op, sizeof(op));
    if (index < 0) return -1;

    // 等待提交并应用到状态机
    return wait_for_apply(store->raft, index);
}

// 读操作 - 在领导者上执行确保线性一致性
int linear_store_read(linear_store_t* store, const char* key,
                      void** value, size_t* len) {
    // 确保本节点是领导者
    if (store->raft->state != STATE_LEADER) {
        return redirect_to_leader(store->raft, key);
    }

    // 提交一个空操作(no-op)来确保所有之前的写入已提交
    raft_append_entry(store->raft, NULL, 0);

    // 从状态机读取
    pthread_mutex_lock(&store->apply_mutex);
    entry_t* entry = hash_table_get(store->data, key);
    pthread_mutex_unlock(&store->apply_mutex);

    if (!entry) return -1;

    *value = malloc(entry->len);
    memcpy(*value, entry->data, entry->len);
    *len = entry->len;
    return 0;
}
```

#### 因果一致性实现

```c
// 向量时钟实现因果一致性
typedef struct {
    uint64_t* clocks;    // 每个节点的逻辑时钟
    size_t node_count;   // 节点数量
} vector_clock_t;

// 比较向量时钟
typedef enum {
    VC_LESS,      // vc1 < vc2
    VC_GREATER,   // vc1 > vc2
    VC_EQUAL,     // vc1 == vc2
    VC_CONCURRENT // vc1 || vc2 (并发)
} vc_compare_result_t;

vc_compare_result_t vector_clock_compare(const vector_clock_t* vc1,
                                         const vector_clock_t* vc2) {
    bool less = false, greater = false;

    for (size_t i = 0; i < vc1->node_count; i++) {
        if (vc1->clocks[i] < vc2->clocks[i]) {
            less = true;
        } else if (vc1->clocks[i] > vc2->clocks[i]) {
            greater = true;
        }
    }

    if (less && !greater) return VC_LESS;
    if (greater && !less) return VC_GREATER;
    if (!less && !greater) return VC_EQUAL;
    return VC_CONCURRENT;
}

// 带向量时钟的数据版本
typedef struct {
    void* data;
    size_t len;
    vector_clock_t* vc;  // 版本向量时钟
} versioned_data_t;

// 因果一致性写
void causal_write(causal_store_t* store, const char* key,
                  const void* value, size_t len) {
    // 递增本节点的逻辑时钟
    store->local_clock->clocks[store->node_id]++;

    versioned_data_t* new_version = malloc(sizeof(versioned_data_t));
    new_version->data = malloc(len);
    memcpy(new_version->data, value, len);
    new_version->len = len;

    // 复制当前向量时钟
    new_version->vc = vector_clock_copy(store->local_clock);

    // 存储新版本 (可能保留多个版本处理并发)
    store_put_versioned(store, key, new_version);

    // 异步复制到其他节点
    replicate_async(store, key, new_version);
}

// 因果一致性读
versioned_data_t* causal_read(causal_store_t* store, const char* key) {
    version_list_t* versions = store_get_versions(store, key);

    // 返回最新的因果依赖版本
    // 如果有并发版本，可能需要合并或返回所有版本
    return select_causal_latest(versions, store->local_clock);
}
```

#### 最终一致性实现

```c
// 基于 Gossip 协议的最终一致性
typedef struct {
    char key[256];
    void* value;
    size_t len;
    uint64_t version;     // 版本号 (通常是时间戳或逻辑时钟)
    uint64_t timestamp;   // 物理时间戳
} gossip_entry_t;

typedef struct {
    gossip_entry_t** entries;
    size_t count;
    size_t capacity;
    pthread_rwlock_t lock;
} gossip_store_t;

// 反熵 Gossip - 定期与其他节点同步
void* anti_entropy_gossip(void* arg) {
    gossip_store_t* store = (gossip_store_t*)arg;

    while (running) {
        sleep(GOSSIP_INTERVAL);

        // 随机选择对等节点
        int peer = select_random_peer();

        // 发送本节点的摘要 (checksum 或版本范围)
        digest_t* local_digest = compute_digest(store);
        send_digest(peer, local_digest);

        // 接收对方的差异请求
        diff_request_t* request = receive_diff_request(peer);

        // 发送对方缺失的条目
        for (size_t i = 0; i < request->missing_count; i++) {
            gossip_entry_t* entry = find_entry(store, request->missing_keys[i]);
            if (entry) {
                send_entry(peer, entry);
            }
        }

        // 接收对方发送的差异条目
        while (has_more_entries(peer)) {
            gossip_entry_t* entry = receive_entry(peer);
            merge_entry(store, entry);
        }
    }
    return NULL;
}

// 合并条目 - 处理冲突
void merge_entry(gossip_store_t* store, gossip_entry_t* remote) {
    pthread_rwlock_wrlock(&store->lock);

    gossip_entry_t* local = find_entry(store, remote->key);

    if (!local) {
        // 本地不存在，直接添加
        add_entry(store, remote);
    } else if (remote->version > local->version) {
        // 远程版本更新，替换
        replace_entry(store, local, remote);
    } else if (remote->version == local->version) {
        // 版本相同但值不同 - 冲突！
        // 使用最后写入胜利 (LWW) 或其他策略
        if (remote->timestamp > local->timestamp) {
            replace_entry(store, local, remote);
        }
    }
    // 否则本地版本更新，忽略远程

    pthread_rwlock_unlock(&store->lock);
}
```

---

## CAP 定理

### CAP 定理概述

CAP 定理指出，在分布式系统中，一致性 (Consistency)、可用性 (Availability)、分区容错性 (Partition Tolerance) 三者不可兼得，最多只能同时满足两项。

```text
                    CAP 定理图示

                         C (一致性)
                        / \
                       /   \
                      /     \
                     /   CA   \      传统关系型数据库
                    /   系统    \     (如单机 MySQL)
                   /             \
          P(分区容错性) ────────── A(可用性)
            /                       \
           /   CP 系统              \   AP 系统
          /   (如 HBase, MongoDB)    \  (如 Cassandra, DynamoDB)
         /                             \
```

### CAP 权衡选择

| 组合 | 特性 | 典型系统 | 适用场景 |
|-----|------|---------|---------|
| CA | 一致且可用，但不能分区 | 单机数据库 | 非分布式场景 |
| CP | 一致且分区容错，但可能不可用 | ZooKeeper, etcd, HBase | 金融交易、配置管理 |
| AP | 可用且分区容错，但可能不一致 | Cassandra, DynamoDB, Eureka | 社交网络、日志收集 |

### CAP 权衡的实际影响

#### CP 系统行为

```c
// CP 系统在分区时的行为：牺牲可用性保证一致性

typedef enum {
    NODE_HEALTHY,
    NODE_PARTITIONED,
    NODE_RECOVERING
} node_health_t;

typedef struct {
    node_health_t health;
    hash_set_t* quorum_members;  // 当前能通信的节点集合
    int quorum_size;             // 达到多数派所需的最小节点数
} cp_node_state_t;

// CP 系统写操作
int cp_system_write(cp_node_state_t* state, const char* key,
                    const void* value) {
    // 检查是否能形成多数派
    if (hash_set_size(state->quorum_members) < state->quorum_size) {
        // 分区发生，无法形成多数派
        // 选择：拒绝操作，牺牲可用性
        return -1;  // 系统不可用
    }

    // 发送给所有可达节点，等待多数派确认
    int acks = 0;
    for (int node : state->quorum_members) {
        if (send_write(node, key, value) == SUCCESS) {
            acks++;
        }
        if (acks >= state->quorum_size) {
            break;
        }
    }

    if (acks >= state->quorum_size) {
        return 0;  // 写入成功
    } else {
        return -1;  // 写入失败，回滚
    }
}
```

#### AP 系统行为

```c
// AP 系统在分区时的行为：牺牲一致性保证可用性

typedef struct {
    bool partitioned;           // 是否处于分区状态
    hash_set_t* reachable_nodes;
    vector_clock_t* local_vc;   // 本地向量时钟
} ap_node_state_t;

// AP 系统写操作
int ap_system_write(ap_node_state_t* state, const char* key,
                    const void* value) {
    // 无论是否分区，都接受写入
    // 生成新的版本
    versioned_value_t* new_val = malloc(sizeof(versioned_value_t));
    new_val->data = malloc(value_len);
    memcpy(new_val->data, value, value_len);
    new_val->vc = vector_clock_increment(state->local_vc, my_node_id);

    // 本地存储
    local_store_put(key, new_val);

    // 异步复制给可达节点
    for (int node : state->reachable_nodes) {
        if (node != my_node_id) {
            async_replicate(node, key, new_val);
        }
    }

    // 立即返回成功 (即使其他分区看不到这个写入)
    return 0;
}

// AP 系统读操作
versioned_value_t* ap_system_read(ap_node_state_t* state, const char* key) {
    // 始终返回本地值，保证可用性
    versioned_value_t* local = local_store_get(key);

    // 异步尝试从其他节点获取更新版本
    for (int node : state->reachable_nodes) {
        async_fetch(node, key);
    }

    return local;
}

// 分区恢复后的冲突解决
void on_partition_heal(ap_node_state_t* state) {
    for (each key in local_store) {
        version_list_t* remote_versions = fetch_versions_from_peers(key);
        version_list_t* local_versions = local_store_get_all_versions(key);

        // 合并版本，解决冲突
        version_list_t* merged = merge_versions(local_versions,
                                                remote_versions);

        // 如果存在并发写入，需要根据业务规则解决
        if (has_concurrent_versions(merged)) {
            resolve_conflicts(key, merged);
        }
    }
}
```

### PACELC 定理

PACELC 是 CAP 的扩展，指出即使没有分区 (P)，也必须在延迟 (L) 和一致性 (C) 之间做权衡。

```text
PACELC:
    如果有分区 (P)，在可用性 (A) 和一致性 (C) 之间选择；
    否则 (E)，在延迟 (L) 和一致性 (C) 之间选择。

示例系统分类:
┌────────────────┬────────────────────────────────────┐
│ 系统           │ PACELC 分类                        │
├────────────────┼────────────────────────────────────┤
│ ZooKeeper      │ PC/EC (分区时选 C，否则也选 C)      │
│ Cassandra      │ PA/EL (分区时选 A，否则选 L)        │
│ MongoDB        │ PA/EC (可配置)                      │
│ DynamoDB       │ PA/EL                               │
│ CockroachDB    │ PC/EC                               │
└────────────────┴────────────────────────────────────┘
```

---

## 分布式事务

### 分布式事务概述

分布式事务涉及多个节点上的操作，需要保证 ACID 特性：原子性 (Atomicity)、一致性 (Consistency)、隔离性 (Isolation)、持久性 (Durability)。

### 两阶段提交 (2PC)

```text
两阶段提交流程:

协调者                          参与者1        参与者2        参与者3
  │                               │              │              │
  ├────────── PREPARE ────────────┼──────────────┼──────────────┤
  │                               │              │              │
  │◄───────── YES/NO ─────────────┤◄─────────────┤◄─────────────┤
  │                               │              │              │
  │  [所有回答 YES?]              │              │              │
  │                               │              │              │
  ├─── 是 ─── COMMIT ─────────────┼──────────────┼──────────────┤
  │                               │              │              │
  │◄──────── ACK ─────────────────┤◄─────────────┤◄─────────────┤
  │                               │              │              │
  ├─── 否 ─── ROLLBACK ───────────┼──────────────┼──────────────┤
  │                               │              │              │
```

#### 2PC 实现

```c
// 两阶段提交 - 协调者实现

typedef enum {
    PC_PHASE_IDLE,
    PC_PHASE_PREPARE,
    PC_PHASE_COMMIT,
    PC_PHASE_ABORT
} pc_phase_t;

typedef struct {
    transaction_id_t tx_id;
    pc_phase_t phase;
    int participant_count;
    int* participant_status;  // 每个参与者的状态
    void* coordinator_log;    // 预写日志
} pc_coordinator_t;

// 第一阶段：准备
int pc_phase_prepare(pc_coordinator_t* coord, transaction_t* tx) {
    coord->phase = PC_PHASE_PREPARE;

    // 记录事务开始日志 (用于故障恢复)
    write_coordinator_log(coord, TX_START, tx);

    int yes_count = 0;
    for (int i = 0; i < coord->participant_count; i++) {
        // 发送 PREPARE 请求
        prepare_request_t req = {
            .tx_id = tx->id,
            .operations = tx->participants[i].operations
        };

        prepare_response_t resp = send_prepare(i, &req);

        if (resp.vote == VOTE_YES) {
            coord->participant_status[i] = STATUS_PREPARED;
            yes_count++;
        } else {
            coord->participant_status[i] = STATUS_VOTED_NO;
            // 收到一个 NO 就立即进入回滚
            return pc_phase_abort(coord, tx);
        }
    }

    // 所有参与者都同意，进入第二阶段
    if (yes_count == coord->participant_count) {
        return pc_phase_commit(coord, tx);
    }

    return -1;
}

// 第二阶段：提交
int pc_phase_commit(pc_coordinator_t* coord, transaction_t* tx) {
    coord->phase = PC_PHASE_COMMIT;

    // 先记录决定 (重要！这是 2PC 的要点)
    write_coordinator_log(coord, TX_COMMIT, tx);

    // 发送 COMMIT 给所有参与者
    for (int i = 0; i < coord->participant_count; i++) {
        commit_request_t req = { .tx_id = tx->id };
        send_commit(i, &req);
        coord->participant_status[i] = STATUS_COMMITTED;
    }

    // 等待 ACK (可异步)
    wait_for_acks(coord, tx->id);

    // 记录完成
    write_coordinator_log(coord, TX_COMPLETE, tx);
    coord->phase = PC_PHASE_IDLE;

    return 0;
}

// 参与者实现
typedef struct {
    transaction_id_t tx_id;
    void* undo_log;      // 回滚日志
    void* redo_log;      // 重做日志
    bool prepared;
} participant_tx_state_t;

prepare_response_t participant_handle_prepare(int participant_id,
                                               prepare_request_t* req) {
    prepare_response_t resp = { .tx_id = req->tx_id };

    // 检查能否执行操作
    if (!can_execute_operations(req->operations)) {
        resp.vote = VOTE_NO;
        return resp;
    }

    // 预执行操作，写入 undo/redo 日志
    participant_tx_state_t* state = malloc(sizeof(participant_tx_state_t));
    state->tx_id = req->tx_id;
    state->undo_log = create_undo_log(req->operations);
    state->redo_log = create_redo_log(req->operations);
    state->prepared = true;

    // 强制刷盘，确保持久化
    fsync(state->undo_log);
    fsync(state->redo_log);

    save_participant_state(state);

    resp.vote = VOTE_YES;
    return resp;
}

void participant_handle_commit(int participant_id, commit_request_t* req) {
    participant_tx_state_t* state = load_participant_state(req->tx_id);

    if (state && state->prepared) {
        // 应用 redo 日志完成事务
        apply_redo_log(state->redo_log);

        // 清理状态
        delete_participant_state(req->tx_id);
    }
}

void participant_handle_rollback(int participant_id,
                                  rollback_request_t* req) {
    participant_tx_state_t* state = load_participant_state(req->tx_id);

    if (state && state->prepared) {
        // 应用 undo 日志回滚事务
        apply_undo_log(state->undo_log);

        // 清理状态
        delete_participant_state(req->tx_id);
    }
}
```

### 三阶段提交 (3PC)

3PC 在 2PC 基础上增加了一个预提交阶段，解决协调者单点故障导致的阻塞问题。

```text
三阶段提交流程:

协调者                 参与者
  │                      │
  ├──── CAN_COMMIT ─────►│  第一阶段：询问是否可以执行
  │◄──── YES/NO ─────────┤
  │                      │
  ├──── PRE_COMMIT ─────►│  第二阶段：预提交
  │◄──── ACK ────────────┤
  │                      │
  ├──── DO_COMMIT ──────►│  第三阶段：正式提交
  │◄──── ACK ────────────┤
```

### Saga 模式

Saga 是长事务的替代方案，将大事务拆分为多个本地事务，通过补偿操作保证最终一致性。

```c
// Saga 模式实现

typedef enum {
    SAGA_STEP_PENDING,
    SAGA_STEP_SUCCEEDED,
    SAGA_STEP_FAILED,
    SAGA_STEP_COMPENSATED
} saga_step_status_t;

typedef struct {
    const char* name;
    int (*action)(void* context);       // 正向操作
    int (*compensate)(void* context);   // 补偿操作
    void* context;
    saga_step_status_t status;
} saga_step_t;

typedef struct {
    saga_step_t* steps;
    size_t step_count;
    size_t current_step;
    saga_status_t status;
} saga_t;

// 执行 Saga
int saga_execute(saga_t* saga) {
    saga->status = SAGA_RUNNING;

    for (size_t i = 0; i < saga->step_count; i++) {
        saga->current_step = i;
        saga_step_t* step = &saga->steps[i];

        step->status = SAGA_STEP_PENDING;

        // 执行正向操作
        int result = step->action(step->context);

        if (result == 0) {
            step->status = SAGA_STEP_SUCCEEDED;
        } else {
            step->status = SAGA_STEP_FAILED;
            saga->status = SAGA_FAILED;

            // 执行补偿操作
            return saga_compensate(saga, i);
        }
    }

    saga->status = SAGA_SUCCEEDED;
    return 0;
}

// 补偿操作
int saga_compensate(saga_t* saga, size_t failed_step) {
    // 对已经成功执行的步骤逆序执行补偿
    for (int i = (int)failed_step - 1; i >= 0; i--) {
        saga_step_t* step = &saga->steps[i];

        if (step->status == SAGA_STEP_SUCCEEDED) {
            int result = step->compensate(step->context);

            if (result == 0) {
                step->status = SAGA_STEP_COMPENSATED;
            } else {
                // 补偿失败，需要人工介入或重试
                saga->status = SAGA_COMPENSATION_FAILED;
                return -1;
            }
        }
    }

    saga->status = SAGA_COMPENSATED;
    return 0;
}

// 订单处理 Saga 示例
saga_t* create_order_saga(order_context_t* ctx) {
    saga_step_t steps[] = {
        {
            .name = "create_order",
            .action = step_create_order,
            .compensate = compensate_create_order,
            .context = ctx
        },
        {
            .name = "reserve_inventory",
            .action = step_reserve_inventory,
            .compensate = compensate_reserve_inventory,
            .context = ctx
        },
        {
            .name = "process_payment",
            .action = step_process_payment,
            .compensate = compensate_process_payment,
            .context = ctx
        },
        {
            .name = "ship_order",
            .action = step_ship_order,
            .compensate = compensate_ship_order,
            .context = ctx
        }
    };

    saga_t* saga = malloc(sizeof(saga_t));
    saga->steps = steps;
    saga->step_count = 4;
    saga->current_step = 0;
    saga->status = SAGA_PENDING;

    return saga;
}

// 具体步骤实现示例
int step_reserve_inventory(void* context) {
    order_context_t* ctx = (order_context_t*)context;

    // 调用库存服务预留库存
    inventory_response_t resp = inventory_service_reserve(
        ctx->product_id,
        ctx->quantity
    );

    if (resp.success) {
        ctx->reservation_id = resp.reservation_id;
        return 0;
    }
    return -1;
}

int compensate_reserve_inventory(void* context) {
    order_context_t* ctx = (order_context_t*)context;

    // 释放预留的库存
    inventory_service_release(ctx->reservation_id);
    return 0;
}
```

### 分布式事务方案对比

| 方案 | 一致性 | 性能 | 复杂度 | 适用场景 |
|-----|-------|------|-------|---------|
| 2PC | 强一致性 | 低 | 中 | 短事务、强一致要求 |
| 3PC | 强一致性 | 较低 | 高 | 对可用性要求更高 |
| Saga | 最终一致性 | 高 | 中 | 长事务、微服务 |
| TCC | 最终一致性 | 高 | 高 | 高并发场景 |
| 本地消息表 | 最终一致性 | 高 | 中 | 异步场景 |
| 最大努力通知 | 弱一致性 | 极高 | 低 | 对一致性要求低 |

---

## 总结

分布式系统的核心挑战在于如何在网络延迟、节点故障、分区等不确定因素下保持一致性和可用性。理解这些概念有助于：

1. **选择合适的架构** - 根据业务需求在 CAP 中做出权衡
2. **设计可靠的系统** - 使用共识算法保证数据一致性
3. **处理并发冲突** - 选择适当的一致性模型
4. **管理分布式事务** - 在性能和一致性之间找到平衡


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
