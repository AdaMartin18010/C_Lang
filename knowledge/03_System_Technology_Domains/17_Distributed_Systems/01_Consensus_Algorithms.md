# 分布式共识算法

> **层级定位**: 03 System Technology Domains / 17 Distributed Systems
> **参考课程**: MIT 6.824, Stanford CS244b
> **难度级别**: L5 专家
> **预估学习时间**: 15-20 小时

---

## 目录

- [分布式共识算法](#分布式共识算法)
  - [目录](#目录)
  - [1. 分布式系统基础](#1-分布式系统基础)
    - [1.1 CAP定理](#11-cap定理)
    - [1.2 故障模型](#12-故障模型)
  - [2. 两阶段提交 (2PC)](#2-两阶段提交-2pc)
    - [2.1 2PC协议](#21-2pc协议)
    - [2.2 2PC实现](#22-2pc实现)
    - [2.3 2PC问题](#23-2pc问题)
  - [3. Raft共识算法](#3-raft共识算法)
    - [3.1 Raft基础](#31-raft基础)
    - [3.2 Raft实现](#32-raft实现)
  - [4. Paxos算法](#4-paxos算法)
    - [4.1 Paxos基础](#41-paxos基础)
    - [4.2 Paxos与Raft对比](#42-paxos与raft对比)
  - [5. 拜占庭容错 (BFT)](#5-拜占庭容错-bft)
    - [5.1 PBFT算法](#51-pbft算法)
  - [关联导航](#关联导航)
    - [前置知识](#前置知识)
    - [后续延伸](#后续延伸)
    - [参考](#参考)

## 1. 分布式系统基础

### 1.1 CAP定理

```
CAP定理 (Brewer定理):

在分布式数据存储系统中，不可能同时满足以下三个特性：

┌─────────────────────────────────────────────────────────────┐
│  C - Consistency (一致性)                                    │
│  • 所有节点在同一时间看到相同的数据                          │
│  • 读操作返回最新的写操作结果                                │
├─────────────────────────────────────────────────────────────┤
│  A - Availability (可用性)                                   │
│  • 每个请求都能收到响应（成功或失败）                        │
│  • 不保证数据是最新的                                        │
├─────────────────────────────────────────────────────────────┤
│  P - Partition Tolerance (分区容错性)                        │
│  • 系统在网络分区情况下仍能继续运行                          │
│  • 分布式系统必须满足                                        │
└─────────────────────────────────────────────────────────────┘

必须选择：
• CP系统：一致性优先 (ZooKeeper, etcd, HBase)
• AP系统：可用性优先 (Cassandra, DynamoDB, DNS)
```

### 1.2 故障模型

```
故障分类：

┌─────────────────────────────────────────────────────────────┐
│  故障类型         │  描述              │  处理难度          │
├───────────────────┼────────────────────┼────────────────────┤
│  Crash-Stop       │ 节点崩溃并停止     │ 简单               │
│  Crash-Recovery   │ 崩溃后恢复         │ 中等               │
│  Omission         │ 遗漏消息           │ 中等               │
│  Timing           │ 时间错误           │ 难                 │
│  Byzantine        │ 任意故障/恶意      │ 最难               │
└───────────────────┴────────────────────┴────────────────────┘

拜占庭故障：
• 节点可能发送错误信息
• 可能勾结欺骗其他节点
• 解决方案：拜占庭容错(BFT)算法
• 需要至少3f+1个节点容忍f个拜占庭故障
```

---

## 2. 两阶段提交 (2PC)

### 2.1 2PC协议

```
两阶段提交协议：

协调者                          参与者
  │                               │
  │  1. Prepare请求              │
  │──────────────────────────────>│
  │  (询问是否可以提交)           │
  │                               │
  │<──────────────────────────────│
  │  2. Prepare响应              │
  │  (Yes/No)                    │
  │                               │
  │  3. Commit/Rollback          │
  │──────────────────────────────>│
  │                               │
  │<──────────────────────────────│
  │  4. Ack                      │

阶段1 - 投票阶段：
• 协调者发送Prepare给所有参与者
• 参与者执行本地事务，锁定资源
• 参与者回复Yes（可以提交）或No（需要回滚）

阶段2 - 提交阶段：
• 如果所有参与者回复Yes，协调者发送Commit
• 如果有任何No，协调者发送Rollback
• 参与者执行提交或回滚，释放锁
```

### 2.2 2PC实现

```c
// 2PC参与者状态机
typedef enum {
    STATE_IDLE,
    STATE_PREPARED,
    STATE_COMMITTED,
    STATE_ABORTED
} participant_state_t;

typedef struct {
    int participant_id;
    participant_state_t state;
    transaction_t *pending_txn;
} participant_t;

// 处理Prepare请求
int handle_prepare(participant_t *p, transaction_t *txn) {
    // 尝试在本地执行事务（不提交）
    if (validate_and_lock(txn) != 0) {
        // 无法执行，回复No
        send_response(p->coordinator, VOTE_NO);
        p->state = STATE_ABORTED;
        return -1;
    }

    // 保存事务，准备提交
    p->pending_txn = txn;
    p->state = STATE_PREPARED;

    // 写预写日志(WAL)
    write_wal(p->log, PREPARE_RECORD, txn);

    // 回复Yes
    send_response(p->coordinator, VOTE_YES);
    return 0;
}

// 处理Commit请求
int handle_commit(participant_t *p, transaction_t *txn) {
    if (p->state != STATE_PREPARED) {
        return -1;
    }

    // 提交事务
    commit_transaction(txn);

    // 写日志
    write_wal(p->log, COMMIT_RECORD, txn);

    // 释放锁
    release_locks(txn);

    p->state = STATE_COMMITTED;
    p->pending_txn = NULL;

    // 确认
    send_response(p->coordinator, ACK);
    return 0;
}

// 恢复处理
define RECOVERY_TIMEOUT 30000  // 30秒

void recover_participant(participant_t *p) {
    // 读取WAL
    log_entry_t *entry = read_wal(p->log);

    switch (entry->type) {
    case PREPARE_RECORD:
        // 已经准备好，但不知道协调者决定
        // 询问协调者或等待
        p->state = STATE_PREPARED;
        // 阻塞直到知道决定
        wait_for_decision(p->coordinator, entry->txn_id, RECOVERY_TIMEOUT);
        break;

    case COMMIT_RECORD:
        // 已经提交，可能协调者没收到确认
        p->state = STATE_COMMITTED;
        // 重发确认
        send_response(p->coordinator, ACK);
        break;

    case ROLLBACK_RECORD:
        p->state = STATE_ABORTED;
        break;
    }
}
```

### 2.3 2PC问题

```
2PC的问题：

1. 同步阻塞
   • 参与者需要锁定资源直到收到Commit/Rollback
   • 如果协调者崩溃，参与者需要等待恢复

2. 单点故障
   • 协调者崩溃可能导致协议挂起
   • 参与者无限期等待

3. 不一致风险
   • 协调者在Commit发送后崩溃
   • 部分参与者收到Commit，部分没有

解决方案 - 三阶段提交(3PC)：
• 增加CanCommit预检查阶段
• 引入超时机制
• 协调者宕机后可以通过超时继续
• 但仍有不一致风险，且复杂度高
```

---

## 3. Raft共识算法

### 3.1 Raft基础

```
Raft设计目标：可理解性

核心概念：
┌─────────────────────────────────────────────────────────────┐
│  Leader选举                                                 │
│  • 每个任期内最多一个Leader                                  │
│  • Leader负责处理所有客户端请求                              │
│  • 超时触发选举                                              │
├─────────────────────────────────────────────────────────────┤
│  日志复制                                                   │
│  • Leader接收客户端请求，追加到本地日志                      │
│  • 复制日志条目到所有Follower                                │
│  • 大多数节点确认后提交                                      │
├─────────────────────────────────────────────────────────────┤
│  安全性                                                     │
│  • 已提交条目不会丢失                                        │
│  • 所有节点看到相同的已提交条目                              │
│  • Leader必须包含所有已提交条目                              │
└─────────────────────────────────────────────────────────────┘

Raft状态：
• Follower: 被动，响应请求
• Candidate: 主动发起选举
• Leader: 处理所有客户端请求
```

### 3.2 Raft实现

```c
// Raft节点状态
typedef enum { STATE_FOLLOWER, STATE_CANDIDATE, STATE_LEADER } node_state_t;

typedef struct {
    int node_id;
    node_state_t state;

    // 持久状态
    int current_term;           // 当前任期
    int voted_for;              // 当前任期投票给谁
    log_entry_t log[MAX_LOG];   // 日志条目
    int log_count;

    // 易失状态
    int commit_index;           // 已知提交的最高索引
    int last_applied;           // 已应用到状态机的最高索引

    // Leader特有状态
    int next_index[MAX_NODES];  // 每个节点的下一个日志索引
    int match_index[MAX_NODES]; // 每个节点已知的最高匹配索引

    // 定时器
    int election_timeout;
    int heartbeat_interval;
} raft_node_t;

// 请求投票 RPC
typedef struct {
    int term;           // 候选人任期
    int candidate_id;   // 候选人ID
    int last_log_index; // 候选人最后日志索引
    int last_log_term;  // 候选人最后日志任期
} request_vote_args_t;

typedef struct {
    int term;           // 当前任期（用于候选人更新自己）
    bool vote_granted;  // 是否投票
} request_vote_reply_t;

// 处理投票请求
request_vote_reply_t handle_request_vote(raft_node_t *node, request_vote_args_t *args) {
    request_vote_reply_t reply = {.term = node->current_term};

    // 任期过时
    if (args->term < node->current_term) {
        reply.vote_granted = false;
        return reply;
    }

    // 更新任期
    if (args->term > node->current_term) {
        node->current_term = args->term;
        node->voted_for = -1;
        become_follower(node);
    }

    // 检查是否可以投票
    if ((node->voted_for == -1 || node->voted_for == args->candidate_id) &&
        is_log_up_to_date(node, args->last_log_index, args->last_log_term)) {
        reply.vote_granted = true;
        node->voted_for = args->candidate_id;
        reset_election_timer(node);
    } else {
        reply.vote_granted = false;
    }

    return reply;
}

// 附加日志 RPC
typedef struct {
    int term;
    int leader_id;
    int prev_log_index;
    int prev_log_term;
    log_entry_t entries[MAX_ENTRIES];
    int entry_count;
    int leader_commit;
} append_entries_args_t;

// Leader发送心跳/日志
define HEARTBEAT_INTERVAL 100  // ms

void leader_send_heartbeats(raft_node_t *node) {
    for (int i = 0; i < MAX_NODES; i++) {
        if (i == node->node_id) continue;

        append_entries_args_t args = {
            .term = node->current_term,
            .leader_id = node->node_id,
            .prev_log_index = node->next_index[i] - 1,
            .prev_log_term = get_log_term(node, node->next_index[i] - 1),
            .leader_commit = node->commit_index
        };

        // 复制待发送的日志条目
        int start = node->next_index[i];
        args.entry_count = node->log_count - start;
        for (int j = 0; j < args.entry_count; j++) {
            args.entries[j] = node->log[start + j];
        }

        send_append_entries(i, &args);
    }
}

// 处理附加日志
append_entries_reply_t handle_append_entries(raft_node_t *node, append_entries_args_t *args) {
    append_entries_reply_t reply = {.term = node->current_term};

    // 任期过时
    if (args->term < node->current_term) {
        reply.success = false;
        return reply;
    }

    // 收到Leader心跳，重置选举定时器
    reset_election_timer(node);

    if (args->term > node->current_term) {
        node->current_term = args->term;
        node->voted_for = -1;
    }

    become_follower(node);

    // 检查日志匹配
    if (args->prev_log_index >= 0) {
        if (get_log_term(node, args->prev_log_index) != args->prev_log_term) {
            reply.success = false;
            // Leader将递减next_index重试
            return reply;
        }
    }

    // 追加新条目
    int insert_index = args->prev_log_index + 1;
    for (int i = 0; i < args->entry_count; i++) {
        if (insert_index + i < node->log_count &&
            node->log[insert_index + i].term != args->entries[i].term) {
            // 冲突，删除后续所有条目
            node->log_count = insert_index + i;
        }

        if (insert_index + i >= node->log_count) {
            // 追加新条目
            node->log[node->log_count++] = args->entries[i];
        }
    }

    // 更新commit_index
    if (args->leader_commit > node->commit_index) {
        node->commit_index = min(args->leader_commit, node->log_count - 1);
    }

    reply.success = true;
    return reply;
}

// Leader提交检查
void leader_commit_entries(raft_node_t *node) {
    // 从后向前找到可以提交的条目
    for (int i = node->commit_index + 1; i < node->log_count; i++) {
        if (node->log[i].term != node->current_term) {
            continue;  // 只提交当前任期的条目
        }

        int match_count = 1;  // Leader自己
        for (int j = 0; j < MAX_NODES; j++) {
            if (j != node->node_id && node->match_index[j] >= i) {
                match_count++;
            }
        }

        // 大多数匹配
        if (match_count > MAX_NODES / 2) {
            node->commit_index = i;
            apply_to_state_machine(node->log[i]);
        }
    }
}
```

---

## 4. Paxos算法

### 4.1 Paxos基础

```
Paxos角色：

Proposer (提议者):
• 接收客户端请求
• 提出值 (Value)
• 执行两阶段协议

Acceptor (接受者):
• 存储接受的值
• 投票决定是否接受提议
• 需要多数派

Learner (学习者):
• 学习被选定的值
• 不直接参与协议

两阶段协议：

Phase 1 - Prepare:
Proposer -> Acceptor: Prepare(n)  // n是提议编号
Acceptor -> Proposer: Promise(n, (n', v'))  // 承诺不接受更小编号，返回已接受的值

Phase 2 - Accept:
Proposer -> Acceptor: Accept(n, v)  // v是提议的值（可能是之前接受的值）
Acceptor -> Proposer: Accepted(n, v)
```

### 4.2 Paxos与Raft对比

```
┌─────────────────────────────────────────────────────────────┐
│              Paxos vs Raft 对比                              │
├─────────────────────────────────────────────────────────────┤
│  特性              │  Paxos              │  Raft             │
├────────────────────┼─────────────────────┼───────────────────┤
│  理解难度          │  困难               │  相对容易         │
│  Leader选举        │  隐式               │  显式             │
│  日志连续性        │  允许空洞           │  连续             │
│  成员变更          │  复杂               │  联合共识         │
│  工程实现          │  较少               │  丰富(etcd等)     │
└────────────────────┴─────────────────────┴───────────────────┘

共同点：
• 都需要多数派确认
• 都保证安全性
• 都不能保证活性（需要超时机制）
```

---

## 5. 拜占庭容错 (BFT)

### 5.1 PBFT算法

```
PBFT (Practical Byzantine Fault Tolerance):

假设：
• n个节点，最多f个拜占庭故障
• n >= 3f + 1

三阶段协议：

Request ->

Pre-Prepare:
  Leader发送Pre-Prepare消息给所有节点
  <PRE-PREPARE, v, n, d>, m
  v: 视图编号, n: 序列号, d: 消息摘要, m: 消息

Prepare:
  节点收到Pre-Prepare后，广播Prepare
  <PREPARE, v, n, d, i>
  i: 节点ID

  当节点收到2f个匹配的Prepare时，进入prepared状态

Commit:
  节点广播Commit
  <COMMIT, v, n, d, i>

  当收到2f+1个匹配的Commit时，提交

Reply:
  向客户端回复

视图变更：
  Leader故障时触发
  新Leader由View = v + 1 mod n确定
```

---

## 关联导航

### 前置知识

- [并发编程](../14_Concurrency_Parallelism/readme.md)
- [网络编程](../15_Network_Programming/readme.md)

### 后续延伸

- [分布式事务](../17_Distributed_Systems/02_Distributed_Transactions.md)
- [分布式一致性](../17_Distributed_Systems/03_Distributed_Consistency.md)

### 参考

- Designing Data-Intensive Applications, Martin Kleppmann
- MIT 6.824: Distributed Systems
- In Search of an Understandable Consensus Algorithm (Raft paper)
