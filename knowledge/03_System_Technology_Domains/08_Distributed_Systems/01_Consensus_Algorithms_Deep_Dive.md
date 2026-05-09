# 分布式共识算法深度剖析

> **层级**: L5 (原理层)
> **目标**: 系统性掌握分布式共识理论，理解Paxos、Raft等算法的原理与实现
> **关联**: [分布式系统基础](../08_Distributed_Systems/README.md) | [网络编程](../05_Network_Programming/README.md)

---

## 1. 分布式系统基础理论

### 1.1 CAP定理与分布式权衡

```
┌─────────────────────────────────────────────────────────────┐
│              CAP定理：分布式系统的基本约束                   │
├─────────────────────────────────────────────────────────────┤
│                                                              │
│                     C ───── A                                │
│                    /    \   /                                │
│                   /      \ /                                 │
│                  /        X                                  │
│                 /        / \                                 │
│                P────────    \                                │
│                                                              │
│  C - Consistency (一致性)                                    │
│      所有节点在同一时刻看到相同的数据                        │
│      线性一致性 (Linearizability): 最强一致性模型           │
│                                                              │
│  A - Availability (可用性)                                   │
│      每个请求都能在有限时间内获得响应                        │
│      不保证获取最新数据，但保证响应                          │
│                                                              │
│  P - Partition Tolerance (分区容错性)                        │
│      系统在网络分区时仍能继续运行                            │
│      分区容错是分布式系统的基本要求                          │
│                                                              │
│  定理: 网络分区时(P)，系统只能保证C或A，不能同时保证两者     │
│                                                              │
│  实际系统选择：                                               │
│  ┌────────────────┬─────────────────────────────────────┐   │
│  │  CP系统        │ 强调一致性，牺牲可用性               │   │
│  │                │ ZooKeeper, etcd, Consul, HBase      │   │
│  ├────────────────┼─────────────────────────────────────┤   │
│  │  AP系统        │ 强调可用性，牺牲强一致性             │   │
│  │                │ Cassandra, DynamoDB, Couchbase      │   │
│  ├────────────────┼─────────────────────────────────────┤   │
│  │  灵活系统      │ 可配置一致性级别                     │   │
│  │                │ MongoDB (可配置), CockroachDB       │   │
│  └────────────────┴─────────────────────────────────────┘   │
│                                                              │
└─────────────────────────────────────────────────────────────┘
```

### 1.2 FLP不可能结果

```
┌─────────────────────────────────────────────────────────────┐
│           FLP不可能结果 (Fischer-Lynch-Paterson)             │
├─────────────────────────────────────────────────────────────┤
│                                                              │
│  定理陈述：                                                   │
│  在异步系统中，即使只有一个故障节点(f=1)，                   │
│  也不存在确定性的共识算法。                                   │
│                                                              │
│  证明核心思想：                                               │
│  ┌─────────────────────────────────────────────────────┐   │
│  │                                                     │   │
│  │  初始状态 (bivalent)                                │   │
│  │  ┌─────────┐   ┌─────────┐   ┌─────────┐           │   │
│  │  │ Node 0  │   │ Node 1  │   │ Node 2  │           │   │
│  │  │ Value=? │   │ Value=? │   │ Value=? │           │   │
│  │  └─────────┘   └─────────┘   └─────────┘           │   │
│  │                                                     │   │
│  │  关键洞察:                                          │   │
│  │  1. 系统从"双值状态"(可决定0或1)开始               │   │
│  │  2. 每个执行步骤最多将系统转为"单值状态"           │   │
│  │  3. 但异步网络中无法确定哪个步骤是关键步骤         │   │
│  │  4. 如果恰好延迟了关键步骤的消息，系统永远         │   │
│  │     保持双值状态，无法达成共识                     │   │
│  │                                                     │   │
│  └─────────────────────────────────────────────────────┘   │
│                                                              │
│  实际意义：                                                   │
│  • 所有实用的共识算法都必须打破FLP假设                       │
│  • 常见打破方式：                                            │
│    - 使用随机化 (Randomized consensus)                       │
│    - 使用故障检测器 (Failure detectors)                      │
│    - 使用超时 (实际系统中常用)                               │
│    - 限制网络异步程度                                        │
│                                                              │
└─────────────────────────────────────────────────────────────┘
```

---

## 2. 共识问题定义

### 2.1 共识的形式化定义

```
┌─────────────────────────────────────────────────────────────┐
│              分布式共识问题定义                              │
├─────────────────────────────────────────────────────────────┤
│                                                              │
│  场景: N个进程(节点)，其中最多f个可能故障                    │
│  每个进程提出一个提议值，所有非故障进程必须达成一致          │
│                                                              │
│  安全性 (Safety) 属性：                                       │
│  ┌─────────────────────────────────────────────────────┐   │
│  │                                                     │   │
│  │  1. 一致性 (Agreement)                              │   │
│  │     没有两个非故障进程决定不同的值                  │   │
│  │                                                     │   │
│  │  2. 有效性 (Validity)                               │   │
│  │     如果所有非故障进程提出相同的值v，              │   │
│  │     那么没有非故障进程会决定不同于v的值            │   │
│  │     (或: 决定的值必须是某个进程提出的值)           │   │
│  │                                                     │   │
│  │  3. 终止性 (Termination)                            │   │
│  │     每个非故障进程最终都会决定某个值               │   │
│  │                                                     │   │
│  └─────────────────────────────────────────────────────┘   │
│                                                              │
│  故障模型：                                                   │
│  ┌────────────────┬─────────────────────────────────────┐   │
│  │ 崩溃-停止      │ 节点故障后停止响应                  │   │
│  │ (Crash-stop)   │ 最简单模型                          │   │
│  ├────────────────┼─────────────────────────────────────┤   │
│  │ 崩溃-恢复      │ 节点可崩溃后恢复，可能丢失状态      │   │
│  │ (Crash-recovery│ 需要持久化存储                      │   │
│  ├────────────────┼─────────────────────────────────────┤   │
│  │ 拜占庭故障     │ 节点可任意行为，包括恶意            │   │
│  │ (Byzantine)    │ PBFT等算法处理                      │   │
│  └────────────────┴─────────────────────────────────────┘   │
│                                                              │
│  容错界限：                                                   │
│  • 崩溃故障: 需要 N ≥ 2f + 1 (多数派机制)                    │
│  • 拜占庭故障: 需要 N ≥ 3f + 1 (PBFT)                        │
│                                                              │
└─────────────────────────────────────────────────────────────┘
```

### 2.2 两将军问题

```
┌─────────────────────────────────────────────────────────────┐
│              两将军问题：共识的不可能性                      │
├─────────────────────────────────────────────────────────────┤
│                                                              │
│  场景：                                                       │
│  ┌──────────┐                       ┌──────────┐            │
│  │ General A│◄─────────────────────►│ General B│            │
│  │ (攻击方) │     不可靠的信使       │ (攻击方) │            │
│  └──────────┘                       └──────────┘            │
│                                                              │
│  问题：                                                       │
│  A想通知B明天攻击，需要确认B收到消息                         │
│  B收到后需要确认A知道B收到了                                 │
│  A收到确认后需要确认B知道A收到了确认...                      │
│                                                              │
│  结果：无限循环的确认链，无法达成确定性的共识                │
│                                                              │
│  结论：在不可靠通信信道上，两将军问题无解                    │
│                                                              │
│  现实意义：                                                   │
│  • TCP三次握手: 工程妥协，非理论解决                         │
│  • 超时重传: 概率性保证                                      │
│  • 共识算法: 在同步假设下工作                                │
│                                                              │
└─────────────────────────────────────────────────────────────┘
```

---

## 3. Paxos算法

### 3.1 Paxos核心机制

```
┌─────────────────────────────────────────────────────────────┐
│              Paxos算法角色与流程                             │
├─────────────────────────────────────────────────────────────┤
│                                                              │
│  角色定义：                                                   │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐          │
│  │  Proposer   │  │   Acceptor  │  │   Learner   │          │
│  │  (提议者)   │  │  (接受者)   │  │  (学习者)   │          │
│  │             │  │             │  │             │          │
│  │ 发起提议    │  │ 投票决定    │  │ 学习结果    │          │
│  │ 提出值      │  │ 接受/拒绝   │  │ 不参与决策  │          │
│  └─────────────┘  └─────────────┘  └─────────────┘          │
│                                                              │
│  两阶段提交：                                                 │
│  ┌─────────────────────────────────────────────────────┐   │
│  │                                                     │   │
│  │  Phase 1: Prepare (承诺阶段)                        │   │
│  │  ────────────────────────                           │   │
│  │                                                     │   │
│  │  Proposer                    Acceptor               │   │
│  │     │                           │                   │   │
│  │     │  Prepare(n)               │                   │   │
│  │     │──────────────────────────►│                   │   │
│  │     │                           │                   │   │
│  │     │  Promise(n, (n', v'))     │                   │   │
│  │     │◄──────────────────────────│                   │   │
│  │     │  (承诺不接受小于n的提议)  │                   │   │
│  │     │  (返回已接受的最高提议)   │                   │   │
│  │                                                     │   │
│  │  Phase 2: Accept (接受阶段)                         │   │
│  │  ────────────────────────                           │   │
│  │                                                     │   │
│  │     │  Accept(n, v)               │                   │   │
│  │     │──────────────────────────►│                   │   │
│  │     │                           │                   │   │
│  │     │  Accepted(n, v)           │                   │   │
│  │     │◄──────────────────────────│                   │   │
│  │                                                     │   │
│  └─────────────────────────────────────────────────────┘   │
│                                                              │
│  安全性保证：                                                 │
│  • 每个Acceptor只接受一个值 → 保证一致性                   │
│  • Proposer从多数派的Promise中学习已接受的值 → 保证有效性  │
│  • 使用提案编号n打破对称性 → 保证活性                      │
│                                                              │
└─────────────────────────────────────────────────────────────┘
```

### 3.2 Paxos正确性证明

```
┌─────────────────────────────────────────────────────────────┐
│              Paxos安全性证明概要                             │
├─────────────────────────────────────────────────────────────┤
│                                                              │
│  定理1: 一致性 (Agreement)                                   │
│  如果值v被某个Acceptor接受，那么任何被接受的其他值都是v      │
│                                                              │
│  证明：                                                       │
│  设v是第一个被Acceptor接受的值，提案编号为n。                │
│  根据Phase 2规则，Proposer必须在Accept请求中发送(n, v)。     │
│  这个Proposer在Phase 1一定收到了多数派的Promise。            │
│  假设之后有值v' (v'≠v)被接受，提案编号为m > n。              │
│  那么发送Accept(m, v')的Proposer一定收到了多数派的Promise(m)。│
│  由于两个多数派必有交集，交集中的Acceptor既承诺了不接受      │
│  小于m的提议，又在之前接受了(n, v)，这与Phase 2的选择规则    │
│  矛盾(应该选择v而非v')。                                     │
│  因此，v' = v。 ∎                                            │
│                                                              │
│  ───────────────────────────────────────────────────────   │
│                                                              │
│  定理2: 有效性 (Validity)                                    │
│  被接受的值必须是某个Proposer提出的值                        │
│                                                              │
│  证明：直接由协议定义得出。Acceptor只接受Proposer发送的值。  │
│                                                              │
│  ───────────────────────────────────────────────────────   │
│                                                              │
│  活性问题：                                                   │
│  Paxos不保证活性。多个Proposer同时竞争可能导致活锁。         │
│  解决：选举Leader，由Leader串行发起提议。                    │
│                                                              │
└─────────────────────────────────────────────────────────────┘
```

---

## 4. Raft算法

### 4.1 Raft架构设计

```
┌─────────────────────────────────────────────────────────────┐
│              Raft算法架构                                    │
├─────────────────────────────────────────────────────────────┤
│                                                              │
│  设计目标：可理解性优先                                       │
│  "Paxos算法的核心很简单，但完整的Paxos协议非常复杂"          │
│                                                              │
│  核心概念分离：                                               │
│  ┌─────────────────────────────────────────────────────┐   │
│  │                                                     │   │
│  │  1. 领导者选举 (Leader Election)                    │   │
│  │     • 任一时刻只有一个Leader                        │   │
│  │     • Leader负责处理所有客户端请求                  │   │
│  │     • 心跳机制检测Leader故障                        │   │
│  │                                                     │   │
│  │  2. 日志复制 (Log Replication)                      │   │
│  │     • Leader接收客户端命令，追加到本地日志          │   │
│  │     • 并行复制日志到所有Follower                    │   │
│  │     • 当日志在多数派复制成功，提交并应用到状态机    │   │
│  │                                                     │   │
│  │  3. 安全性 (Safety)                                 │   │
│  │     • 选举限制：只有包含所有已提交日志的节点       │   │
│  │       才能成为Leader                                │   │
│  │     • 提交规则：Leader只能提交当前任期的日志       │   │
│  │                                                     │   │
│  └─────────────────────────────────────────────────────┘   │
│                                                              │
│  节点状态：                                                   │
│  ┌──────────┐      ┌──────────┐      ┌──────────┐           │
│  │ Follower │ ───► │ Candidate│ ───► │  Leader  │           │
│  │(被动接收)│      │(选举中)  │      │(处理请求)│           │
│  └──────────┘      └──────────┘      └──────────┘           │
│       ▲                                    │                │
│       └────────────────────────────────────┘                │
│              (发现更高任期或新Leader)                        │
│                                                              │
└─────────────────────────────────────────────────────────────┘
```

### 4.2 Raft领导者选举

```
┌─────────────────────────────────────────────────────────────┐
│              Raft领导者选举流程                              │
├─────────────────────────────────────────────────────────────┤
│                                                              │
│  选举触发：                                                   │
│  • Follower在选举超时(election timeout)内未收到心跳        │
│  • 转换为Candidate，增加任期(term)，发起选举               │
│                                                              │
│  选举流程：                                                   │
│  ┌─────────────────────────────────────────────────────┐   │
│  │                                                     │   │
│  │  Candidate                    Followers             │   │
│  │     │                            │                  │   │
│  │     │  RequestVote(T, Id,        │                  │   │
│  │     │            lastLogIndex,   │                  │   │
│  │     │            lastLogTerm)    │                  │   │
│  │     │════════════════════════════╪═══════════════►  │   │
│  │     │                            │                  │   │
│  │     │  Vote(T, voteGranted)      │                  │   │
│  │     │◄═══════════════════════════╪══════════════════│   │
│  │     │                            │                  │   │
│  │                                                     │   │
│  │  投票条件 (同时满足):                               │   │
│  │  1. term > currentTerm (更新自己的term)            │   │
│  │  2. 未在当前term投过票，或投票给同一Candidate      │   │
│  │  3. Candidate的日志至少和自己一样新                │   │
│  │     (lastLogTerm更大，或相同但lastLogIndex更大)    │   │
│  │                                                     │   │
│  │  赢得选举: 获得多数派(N/2+1)投票                    │   │
│  │  转换为Leader，立即发送空AppendEntries(心跳)       │   │
│  │                                                     │   │
│  └─────────────────────────────────────────────────────┘   │
│                                                              │
│  安全性保证 - 选举限制：                                       │
│  • 只有包含所有已提交日志的节点才能成为Leader                │
│  • 这保证了新Leader不会覆盖已提交的日志                      │
│                                                              │
│  活性保证 - 随机化超时：                                       │
│  • 每个节点的选举超时随机在[T, 2T]之间                       │
│  • 大大降低了多个Candidate同时分裂选票的概率                 │
│                                                              │
└─────────────────────────────────────────────────────────────┘
```

### 4.3 Raft日志复制

```
┌─────────────────────────────────────────────────────────────┐
│              Raft日志复制机制                                │
├─────────────────────────────────────────────────────────────┤
│                                                              │
│  日志结构：                                                   │
│  ┌─────────────────────────────────────────────────────┐   │
│  │  Index │ Term │ Command                               │   │
│  │    1   │   1  │  x = 0                               │   │
│  │    2   │   1  │  y = 3                               │   │
│  │    3   │   2  │  x = 2  ◄── 已提交(应用到状态机)     │   │
│  │    4   │   2  │  y = 1                               │   │
│  │    5   │   3  │  x = 4  ◄── Leader当前任期           │   │
│  └─────────────────────────────────────────────────────┘   │
│                                                              │
│  复制流程：                                                   │
│  ┌─────────────────────────────────────────────────────┐   │
│  │                                                     │   │
│  │  Leader                       Followers             │   │
│  │     │                            │                  │   │
│  │     │ AppendEntries(T, leaderId, │                  │   │
│  │     │   prevLogIndex, prevLogTerm│                  │   │
│  │     │   entries[], leaderCommit) │                  │   │
│  │     │════════════════════════════╪═══════════════►  │   │
│  │     │                            │                  │   │
│  │     │  Success/Failure           │                  │   │
│  │     │◄═══════════════════════════╪══════════════════│   │
│  │     │                            │                  │   │
│  │                                                     │   │
│  │  Follower处理:                                      │   │
│  │  1. term < currentTerm → 拒绝                       │   │
│  │  2. prevLogIndex处无日志或term不匹配 → 拒绝         │   │
│  │  3. 删除冲突的后续日志，追加新entries               │   │
│  │  4. leaderCommit > commitIndex → 提交到leaderCommit│   │
│  │                                                     │   │
│  └─────────────────────────────────────────────────────┘   │
│                                                              │
│  提交规则：                                                   │
│  • 条目在Leader当前任期被复制到多数派 → 可提交             │
│  • 之前任期的条目不能仅通过存储在多数派提交                 │
│    (需要通过当前任期的条目的复制间接提交)                   │
│                                                              │
└─────────────────────────────────────────────────────────────┘
```

### 4.4 Raft C语言实现核心

```c
/* ============================================================
 * Raft算法核心数据结构 (简化版)
 * ============================================================ */

#include <stdint.h>
#include <stdbool.h>
#include <time.h>

/* 日志条目 */
struct raft_entry {
    uint64_t term;           // 任期号
    uint64_t index;          // 日志索引
    void* data;              // 命令数据
    size_t data_len;
};

/* 节点状态 */
enum raft_state {
    RAFT_STATE_FOLLOWER,
    RAFT_STATE_CANDIDATE,
    RAFT_STATE_LEADER
};

/* Raft节点 */
struct raft_node {
    // 持久化状态 (需要持久化到磁盘)
    uint64_t current_term;       // 当前任期
    int voted_for;               // 当前任期投票给谁 (-1表示未投票)
    struct raft_entry* log;      // 日志条目数组
    size_t log_count;

    // 易失性状态
    enum raft_state state;
    int id;                      // 节点ID
    int* peers;                  // 其他节点ID列表
    int peer_count;

    // 易失性状态 (所有节点)
    uint64_t commit_index;       // 已知的最高已提交索引
    uint64_t last_applied;       // 已应用到状态机的最高索引

    // 易失性状态 (仅Leader)
    uint64_t* next_index;        // 每个节点的下一个日志索引
    uint64_t* match_index;       // 每个节点已复制的最高索引

    // 选举超时
    struct timespec last_heartbeat;
    int election_timeout_ms;     // 随机化在[150, 300]ms
};

/* 请求投票 RPC */
struct request_vote_args {
    uint64_t term;           // Candidate的任期
    int candidate_id;        // Candidate的ID
    uint64_t last_log_index; // Candidate最后日志索引
    uint64_t last_log_term;  // Candidate最后日志任期
};

struct request_vote_reply {
    uint64_t term;           // 当前任期(用于Candidate更新自己)
    bool vote_granted;       // 是否投票
};

/* 追加日志 RPC */
struct append_entries_args {
    uint64_t term;           // Leader的任期
    int leader_id;           // Leader的ID
    uint64_t prev_log_index; // 前一个日志索引
    uint64_t prev_log_term;  // 前一个日志任期
    struct raft_entry* entries; // 要追加的条目(心跳时为空)
    size_t entry_count;
    uint64_t leader_commit;  // Leader的commit_index
};

struct append_entries_reply {
    uint64_t term;           // 当前任期
    bool success;            // 是否成功追加
    uint64_t conflict_index; // 冲突时的提示(优化用)
};

/* ============================================================
 * 核心函数
 * ============================================================ */

/* 处理RequestVote RPC */
void raft_handle_request_vote(struct raft_node* node,
                               const struct request_vote_args* args,
                               struct request_vote_reply* reply) {
    reply->vote_granted = false;
    reply->term = node->current_term;

    // 任期更高，转换为Follower
    if (args->term > node->current_term) {
        node->current_term = args->term;
        node->state = RAFT_STATE_FOLLOWER;
        node->voted_for = -1;
        reply->term = node->current_term;
    }

    // 任期过低，拒绝
    if (args->term < node->current_term) {
        return;
    }

    // 检查日志是否至少和自己一样新
    uint64_t last_index = node->log_count > 0 ?
                          node->log[node->log_count-1].index : 0;
    uint64_t last_term = node->log_count > 0 ?
                         node->log[node->log_count-1].term : 0;

    bool log_is_up_to_date =
        (args->last_log_term > last_term) ||
        (args->last_log_term == last_term &&
         args->last_log_index >= last_index);

    // 未投票或已投票给同一Candidate
    if ((node->voted_for == -1 || node->voted_for == args->candidate_id)
        && log_is_up_to_date) {
        node->voted_for = args->candidate_id;
        reply->vote_granted = true;
        raft_reset_election_timer(node);
    }
}

/* 成为Candidate，发起选举 */
void raft_become_candidate(struct raft_node* node) {
    node->state = RAFT_STATE_CANDIDATE;
    node->current_term++;
    node->voted_for = node->id;
    raft_reset_election_timer(node);

    // 并行向所有节点发送RequestVote
    struct request_vote_args args = {
        .term = node->current_term,
        .candidate_id = node->id,
        .last_log_index = node->log_count > 0 ?
                          node->log[node->log_count-1].index : 0,
        .last_log_term = node->log_count > 0 ?
                         node->log[node->log_count-1].term : 0
    };

    for (int i = 0; i < node->peer_count; i++) {
        raft_send_request_vote(node->peers[i], &args);
    }
}

/* 成为Leader */
void raft_become_leader(struct raft_node* node) {
    if (node->state != RAFT_STATE_CANDIDATE) return;

    node->state = RAFT_STATE_LEADER;

    // 初始化Leader状态
    uint64_t last_log_index = node->log_count > 0 ?
                              node->log[node->log_count-1].index + 1 : 1;
    for (int i = 0; i < node->peer_count; i++) {
        node->next_index[i] = last_log_index;
        node->match_index[i] = 0;
    }

    // 立即发送心跳
    raft_send_heartbeats(node);
}

/* Leader追加新条目 */
bool raft_leader_append(struct raft_node* node, const void* data,
                        size_t len) {
    if (node->state != RAFT_STATE_LEADER) return false;

    struct raft_entry entry = {
        .term = node->current_term,
        .index = node->log_count > 0 ?
                 node->log[node->log_count-1].index + 1 : 1,
        .data = malloc(len),
        .data_len = len
    };
    memcpy(entry.data, data, len);

    // 追加到本地日志
    raft_log_append(node, &entry);

    // 异步复制到所有Follower
    raft_replicate_to_followers(node);

    return true;
}

/* 检查是否可以提交 */
void raft_maybe_commit(struct raft_node* node) {
    if (node->state != RAFT_STATE_LEADER) return;

    // 找到最大的N，使得match_index[i] >= N的节点超过半数
    for (uint64_t n = node->commit_index + 1; n <= node->log_count; n++) {
        // 只提交当前任期的条目
        if (node->log[n-1].term != node->current_term) continue;

        int match_count = 1;  // 包括Leader自己
        for (int i = 0; i < node->peer_count; i++) {
            if (node->match_index[i] >= n) match_count++;
        }

        if (match_count > (node->peer_count + 1) / 2) {
            node->commit_index = n;
        }
    }

    // 应用到状态机
    while (node->last_applied < node->commit_index) {
        node->last_applied++;
        raft_apply_to_state_machine(node, &node->log[node->last_applied-1]);
    }
}
```

---

## 5. 共识算法对比

```
┌──────────────────┬──────────┬──────────┬─────────────────────┐
│ 特性             │ Paxos    │ Raft     │ PBFT                │
├──────────────────┼──────────┼──────────┼─────────────────────┤
│ 故障模型         │ 崩溃故障 │ 崩溃故障 │ 拜占庭故障          │
│ 容错数           │ f < N/2  │ f < N/2  │ f < N/3             │
│ 理解难度         │ 困难     │ 简单     │ 中等                │
│ 消息复杂度       │ O(N)     │ O(N)     │ O(N²)               │
│ 领导者           │ 无       │ 有       │ 轮流                │
│ 性能             │ 高       │ 高       │ 较低                │
│ 应用场景         │ Chubby   │ etcd,    │ 区块链              │
│                  │          │ Consul   │ Tendermint          │
├──────────────────┼──────────┼──────────┼─────────────────────┤
│ 缺点             │ 难实现   │ 日志复制 │ 消息多              │
│                  │ 易出错   │ 可能落后 │ 性能差              │
└──────────────────┴──────────┴──────────┴─────────────────────┘
```

---

## 6. 总结

```
┌─────────────────────────────────────────────────────────────┐
│                                                              │
│  分布式共识核心原则：                                         │
│                                                              │
│  1. 多数派原则 - 容忍N/2的节点故障                           │
│  2. 两阶段提交 - Prepare-Promise / Propose-Accept            │
│  3. 任期/视图号 - 打破对称性，解决冲突                       │
│  4. Leader优化 - 串行化提议，提高性能                        │
│  5. 日志连续性 - 保证已提交日志不会被覆盖                    │
│  6. 安全性优先 - 宁可不可用，也不违反一致性                  │
│                                                              │
│  工程实践建议：                                               │
│  • 首选Raft (易理解、易实现、有成熟库)                       │
│  • 使用成熟实现 (etcd, Consul, Raft库)                       │
│  • 充分测试网络分区、节点故障场景                            │
│  • 监控Leader切换频率、日志复制延迟                          │
│                                                              │
└─────────────────────────────────────────────────────────────┘
```

---

*分布式共识是分布式系统的核心问题，理解其原理是构建可靠系统的基础。*
