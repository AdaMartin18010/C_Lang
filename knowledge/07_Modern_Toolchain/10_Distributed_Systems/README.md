# 分布式系统基础

## 概述

分布式系统是由多台独立计算机通过网络连接协同工作的系统。与单机系统相比，分布式系统提供了更高的可用性、可扩展性和容错能力，但也引入了网络延迟、分区容错、一致性等复杂挑战。

---

## 1. CAP 定理

### 1.1 定理阐述

CAP定理指出，分布式系统无法同时满足以下三个特性：

```
                    CAP 定理
                       │
        ┌──────────────┼──────────────┐
        │              │              │
        ▼              ▼              ▼
   ┌─────────┐   ┌─────────┐   ┌─────────┐
   │Consistency│   │Availability│   │Partition  │
   │  一致性   │   │  可用性    │   │Tolerance  │
   │           │   │            │   │分区容错性 │
   └─────────┘   └─────────┘   └─────────┘
        │              │              │
        │              │              │
        └──────────────┴──────────────┘
                       │
              只能同时满足其中两个
```

- **一致性 (Consistency)**: 所有节点在同一时刻看到相同的数据
- **可用性 (Availability)**: 每个请求都能在有限时间内获得响应
- **分区容错性 (Partition Tolerance)**: 系统在网络分区时仍能继续运行

### 1.2 CAP权衡策略

| 组合 | 代表系统 | 适用场景 |
|------|----------|----------|
| CP (Consistency + Partition Tolerance) | HBase, MongoDB, Redis Cluster | 金融交易、库存管理 |
| AP (Availability + Partition Tolerance) | Cassandra, DynamoDB, Couchbase | 社交网络、日志收集 |
| CA (Consistency + Availability) | 传统关系型数据库 | 单机或内部网络系统 |

### 1.3 C语言中的CAP权衡

```c
/* cap_tradeoffs.h - CAP权衡策略定义 */
#ifndef CAP_TRADEOFFS_H
#define CAP_TRADEOFFS_H

typedef enum {
    CAP_STRATEGY_CP,    /* 优先一致性 */
    CAP_STRATEGY_AP,    /* 优先可用性 */
    CAP_STRATEGY_ADJUSTABLE  /* 动态调整 */
} cap_strategy_t;

/* 分布式配置 */
typedef struct {
    cap_strategy_t strategy;
    uint32_t consistency_level;  /* 写入确认节点数 */
    uint32_t replication_factor; /* 副本数 */
    uint32_t read_quorum;        /* 读仲裁数 */
    uint32_t write_quorum;       /* 写仲裁数 */
} distributed_config_t;

/* 仲裁计算 */
static inline uint32_t calculate_quorum(uint32_t replication_factor,
                                         cap_strategy_t strategy) {
    switch (strategy) {
        case CAP_STRATEGY_CP:
            /* CP: 大多数节点 (严格一致性) */
            return (replication_factor / 2) + 1;
        case CAP_STRATEGY_AP:
            /* AP: 最少节点 (最终一致性) */
            return 1;
        default:
            return (replication_factor / 2) + 1;
    }
}

#endif /* CAP_TRADEOFFS_H */
```

---

## 2. 一致性模型

### 2.1 一致性级别

```
一致性强度递减:

强一致性 ──────────────────────────────────────────► 最终一致性
│                                                      │
├─ 线性一致性 (Linearizable)                            ├─ 因果一致性
├─ 顺序一致性 (Sequential)                              ├─ 会话一致性
├─ 因果一致性 (Causal)                                  └─ 单调读/写
└─ 外部一致性 (External)

实现成本: 高 ◄──────────────────────────────────────────► 低
性能:     低 ◄──────────────────────────────────────────► 高
可用性:   低 ◄──────────────────────────────────────────► 高
```

### 2.2 一致性模型C语言实现框架

```c
/* consistency_models.h */
#ifndef CONSISTENCY_MODELS_H
#define CONSISTENCY_MODELS_H

#include <stdint.h>
#include <stdbool.h>
#include <time.h>

/* 一致性级别 */
typedef enum {
    CONSISTENCY_LINEARIZABLE = 0,  /* 线性一致性 - 最强 */
    CONSISTENCY_SEQUENTIAL,        /* 顺序一致性 */
    CONSISTENCY_CAUSAL,            /* 因果一致性 */
    CONSISTENCY_SESSION,           /* 会话一致性 */
    CONSISTENCY_EVENTUAL,          /* 最终一致性 - 最弱 */
    CONSISTENCY_MONOTONIC_READ,    /* 单调读 */
    CONSISTENCY_MONOTONIC_WRITE    /* 单调写 */
} consistency_level_t;

/* 向量时钟 - 用于因果一致性 */
typedef struct {
    uint32_t node_id;
    uint64_t counter;
} vector_clock_entry_t;

typedef struct {
    vector_clock_entry_t *entries;
    size_t count;
    size_t capacity;
} vector_clock_t;

/* 时间戳 - 用于线性一致性 */
typedef struct {
    uint64_t physical_time;   /* 物理时间戳 */
    uint32_t logical_time;    /* 逻辑时间戳 (Lamport时钟) */
    uint32_t node_id;         /* 节点标识 */
} hybrid_timestamp_t;

/* 版本化数据 */
typedef struct {
    void *data;
    size_t data_len;
    vector_clock_t *vc;           /* 向量时钟 */
    hybrid_timestamp_t hts;       /* 混合时间戳 */
    uint64_t version;             /* 单调版本号 */
} versioned_value_t;

/* API声明 */
vector_clock_t *vc_create(size_t initial_capacity);
void vc_destroy(vector_clock_t *vc);
void vc_increment(vector_clock_t *vc, uint32_t node_id);
void vc_merge(vector_clock_t *dest, const vector_clock_t *src);
int vc_compare(const vector_clock_t *a, const vector_clock_t *b);

/* 一致性检查 */
bool check_read_your_writes(versioned_value_t *write_val,
                            versioned_value_t *read_val,
                            uint32_t client_id);
bool check_monotonic_reads(versioned_value_t *prev_read,
                           versioned_value_t *current_read);

#endif /* CONSISTENCY_MODELS_H */
```

```c
/* consistency_models.c - 向量时钟实现 */
#include "consistency_models.h"
#include <stdlib.h>
#include <string.h>

vector_clock_t *vc_create(size_t initial_capacity) {
    vector_clock_t *vc = calloc(1, sizeof(vector_clock_t));
    if (!vc) return NULL;

    vc->entries = calloc(initial_capacity, sizeof(vector_clock_entry_t));
    if (!vc->entries) {
        free(vc);
        return NULL;
    }

    vc->capacity = initial_capacity;
    return vc;
}

void vc_increment(vector_clock_t *vc, uint32_t node_id) {
    if (!vc) return;

    /* 查找现有条目 */
    for (size_t i = 0; i < vc->count; i++) {
        if (vc->entries[i].node_id == node_id) {
            vc->entries[i].counter++;
            return;
        }
    }

    /* 添加新条目 */
    if (vc->count >= vc->capacity) {
        /* 扩容 */
        size_t new_cap = vc->capacity * 2;
        vector_clock_entry_t *new_entries = realloc(vc->entries,
                                                    new_cap * sizeof(vector_clock_entry_t));
        if (!new_entries) return;
        vc->entries = new_entries;
        vc->capacity = new_cap;
    }

    vc->entries[vc->count].node_id = node_id;
    vc->entries[vc->count].counter = 1;
    vc->count++;
}

/* 向量时钟比较
 * 返回值: 1 = a > b, -1 = a < b, 0 = 并发(不可比较)
 */
int vc_compare(const vector_clock_t *a, const vector_clock_t *b) {
    if (!a || !b) return 0;

    bool a_greater = false;
    bool b_greater = false;

    /* 检查所有已知节点 */
    size_t max_nodes = a->count > b->count ? a->count : b->count;

    for (size_t i = 0; i < max_nodes; i++) {
        uint64_t a_val = 0, b_val = 0;

        if (i < a->count) a_val = a->entries[i].counter;
        if (i < b->count) b_val = b->entries[i].counter;

        if (a_val > b_val) a_greater = true;
        if (b_val > a_val) b_greater = true;
    }

    if (a_greater && !b_greater) return 1;
    if (b_greater && !a_greater) return -1;
    return 0; /* 并发或相等 */
}

void vc_merge(vector_clock_t *dest, const vector_clock_t *src) {
    if (!dest || !src) return;

    for (size_t i = 0; i < src->count; i++) {
        uint32_t node_id = src->entries[i].node_id;
        uint64_t counter = src->entries[i].counter;

        /* 查找或创建条目 */
        bool found = false;
        for (size_t j = 0; j < dest->count; j++) {
            if (dest->entries[j].node_id == node_id) {
                if (counter > dest->entries[j].counter) {
                    dest->entries[j].counter = counter;
                }
                found = true;
                break;
            }
        }

        if (!found) {
            vc_increment(dest, node_id);
            dest->entries[dest->count - 1].counter = counter;
        }
    }
}
```

---

## 3. 分布式共识算法

### 3.1 Raft 共识算法

Raft是一种易于理解的共识算法，将一致性问题分解为三个子问题：

```
┌─────────────────────────────────────────────────────────────┐
│                     Raft 共识算法                            │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐      │
│  │ Leader Election│  │   Log Replication   │  │  Safety    │      │
│  │   领导者选举   │  │     日志复制        │  │  安全性    │      │
│  └──────────────┘  └──────────────┘  └──────────────┘      │
│                                                             │
│  角色状态:                                                  │
│  ┌──────────┐      ┌──────────┐      ┌──────────┐          │
│  │ Leader   │◄────►│ Candidate│◄────►│ Follower │          │
│  │  领导者   │      │ 候选人    │      │  追随者   │          │
│  └──────────┘      └──────────┘      └──────────┘          │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 3.2 Raft C语言实现框架

```c
/* raft_consensus.h */
#ifndef RAFT_CONSENSUS_H
#define RAFT_CONSENSUS_H

#include <stdint.h>
#include <stdbool.h>

/* Raft节点角色 */
typedef enum {
    RAFT_ROLE_FOLLOWER = 0,
    RAFT_ROLE_CANDIDATE,
    RAFT_ROLE_LEADER
} raft_role_t;

/* 日志条目 */
typedef struct {
    uint64_t term;          /* 任期号 */
    uint64_t index;         /* 日志索引 */
    uint32_t type;          /* 条目类型 */
    uint8_t *data;          /* 数据 */
    size_t data_len;
} raft_log_entry_t;

/* Raft节点状态 */
typedef struct {
    /* 持久化状态 */
    uint64_t current_term;      /* 当前任期 */
    int32_t voted_for;          /* 投票给谁 (-1表示未投票) */
    raft_log_entry_t *log;      /* 日志条目数组 */
    size_t log_count;
    size_t log_capacity;

    /* 易失状态 */
    raft_role_t role;
    uint32_t node_id;
    uint32_t cluster_size;

    /* 所有节点 */
    uint64_t commit_index;      /* 已提交的最高日志索引 */
    uint64_t last_applied;      /* 已应用到状态机的最高索引 */

    /* 仅领导者 */
    uint64_t *next_index;       /* 每个节点的下一个日志索引 */
    uint64_t *match_index;      /* 每个节点匹配的最高索引 */

    /* 超时控制 */
    uint64_t election_timeout_ms;
    uint64_t heartbeat_interval_ms;
    uint64_t last_heartbeat;
} raft_state_t;

/* RPC请求/响应 */
typedef struct {
    uint64_t term;
    uint32_t candidate_id;
    uint64_t last_log_index;
    uint64_t last_log_term;
} raft_request_vote_req_t;

typedef struct {
    uint64_t term;
    bool vote_granted;
} raft_request_vote_rsp_t;

typedef struct {
    uint64_t term;
    uint32_t leader_id;
    uint64_t prev_log_index;
    uint64_t prev_log_term;
    raft_log_entry_t *entries;
    size_t entry_count;
    uint64_t leader_commit;
} raft_append_entries_req_t;

typedef struct {
    uint64_t term;
    bool success;
    uint64_t match_index;
} raft_append_entries_rsp_t;

/* API */
raft_state_t *raft_create(uint32_t node_id, uint32_t cluster_size);
void raft_destroy(raft_state_t *raft);

/* 状态机 */
void raft_become_follower(raft_state_t *raft, uint64_t term);
void raft_become_candidate(raft_state_t *raft);
void raft_become_leader(raft_state_t *raft);

/* RPC处理 */
raft_request_vote_rsp_t raft_handle_request_vote(raft_state_t *raft,
                                                  const raft_request_vote_req_t *req);
raft_append_entries_rsp_t raft_handle_append_entries(raft_state_t *raft,
                                                      const raft_append_entries_req_t *req);

/* 客户端请求 */
int raft_propose(raft_state_t *raft, const uint8_t *data, size_t len);

/* 定时器处理 */
void raft_on_election_timeout(raft_state_t *raft);
void raft_on_heartbeat_interval(raft_state_t *raft);

#endif /* RAFT_CONSENSUS_H */
```

```c
/* raft_consensus.c - 核心实现 */
#include "raft_consensus.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

raft_state_t *raft_create(uint32_t node_id, uint32_t cluster_size) {
    raft_state_t *raft = calloc(1, sizeof(raft_state_t));
    if (!raft) return NULL;

    raft->node_id = node_id;
    raft->cluster_size = cluster_size;
    raft->role = RAFT_ROLE_FOLLOWER;
    raft->current_term = 0;
    raft->voted_for = -1;
    raft->commit_index = 0;
    raft->last_applied = 0;

    /* 日志初始化 */
    raft->log_capacity = 1024;
    raft->log = calloc(raft->log_capacity, sizeof(raft_log_entry_t));

    /* 领导者状态 */
    raft->next_index = calloc(cluster_size, sizeof(uint64_t));
    raft->match_index = calloc(cluster_size, sizeof(uint64_t));

    /* 超时设置 */
    raft->election_timeout_ms = 150 + (rand() % 150); /* 150-300ms随机 */
    raft->heartbeat_interval_ms = 50;

    return raft;
}

void raft_become_candidate(raft_state_t *raft) {
    if (!raft) return;

    raft->role = RAFT_ROLE_CANDIDATE;
    raft->current_term++;
    raft->voted_for = raft->node_id;

    printf("Node %u became candidate for term %lu\n",
           raft->node_id, raft->current_term);

    /* 向其他节点发送RequestVote RPC */
    raft_request_vote_req_t req = {
        .term = raft->current_term,
        .candidate_id = raft->node_id,
        .last_log_index = raft->log_count > 0 ?
            raft->log[raft->log_count - 1].index : 0,
        .last_log_term = raft->log_count > 0 ?
            raft->log[raft->log_count - 1].term : 0
    };

    /* 这里应该发送RPC到所有其他节点 */
    /* 收集投票，如果获得多数票则成为领导者 */
}

void raft_become_leader(raft_state_t *raft) {
    if (!raft) return;

    raft->role = RAFT_ROLE_LEADER;

    /* 初始化领导者状态 */
    uint64_t last_log = raft->log_count > 0 ?
        raft->log[raft->log_count - 1].index + 1 : 1;

    for (uint32_t i = 0; i < raft->cluster_size; i++) {
        raft->next_index[i] = last_log;
        raft->match_index[i] = 0;
    }

    printf("Node %u became leader for term %lu\n",
           raft->node_id, raft->current_term);

    /* 立即发送心跳 */
    raft_on_heartbeat_interval(raft);
}

raft_request_vote_rsp_t raft_handle_request_vote(raft_state_t *raft,
                                                  const raft_request_vote_req_t *req) {
    raft_request_vote_rsp_t rsp = {
        .term = raft->current_term,
        .vote_granted = false
    };

    /* 任期检查 */
    if (req->term < raft->current_term) {
        return rsp; /* 拒绝 */
    }

    if (req->term > raft->current_term) {
        raft->current_term = req->term;
        raft->voted_for = -1;
        raft_become_follower(raft, req->term);
    }

    /* 投票检查 */
    if ((raft->voted_for == -1 || raft->voted_for == req->candidate_id) &&
        /* 日志至少一样新 */
        (req->last_log_term > (raft->log_count > 0 ?
                               raft->log[raft->log_count - 1].term : 0) ||
         (req->last_log_term == (raft->log_count > 0 ?
                                 raft->log[raft->log_count - 1].term : 0) &&
          req->last_log_index >= raft->log_count))) {

        raft->voted_for = req->candidate_id;
        rsp.vote_granted = true;
        printf("Node %u voted for %u in term %lu\n",
               raft->node_id, req->candidate_id, raft->current_term);
    }

    return rsp;
}

int raft_propose(raft_state_t *raft, const uint8_t *data, size_t len) {
    if (!raft || raft->role != RAFT_ROLE_LEADER) {
        return -1; /* 只有领导者能接收客户端请求 */
    }

    /* 扩容检查 */
    if (raft->log_count >= raft->log_capacity) {
        size_t new_cap = raft->log_capacity * 2;
        raft_log_entry_t *new_log = realloc(raft->log,
                                            new_cap * sizeof(raft_log_entry_t));
        if (!new_log) return -1;
        raft->log = new_log;
        raft->log_capacity = new_cap;
    }

    /* 创建新日志条目 */
    raft_log_entry_t *entry = &raft->log[raft->log_count];
    entry->term = raft->current_term;
    entry->index = raft->log_count + 1;
    entry->data = malloc(len);
    memcpy(entry->data, data, len);
    entry->data_len = len;

    raft->log_count++;

    /* 异步复制到跟随者 */
    /* 等待多数确认后提交 */

    return 0;
}
```

### 3.3 Paxos vs Raft 对比

| 特性 | Paxos | Raft |
|------|-------|------|
| 可理解性 | 复杂，难以实现 | 设计目标就是易于理解 |
| 领导者 | Multi-Paxos有领导者 | 必须有领导者 |
| 成员变更 | 复杂 | 支持联合共识 |
| 性能 | 类似 | 类似 |
| 工业应用 | Chubby, PaxosStore | etcd, Consul, TiKV |

---

## 4. 分布式事务

### 4.1 两阶段提交 (2PC)

```
┌─────────────────────────────────────────────────────────────┐
│                     两阶段提交 (2PC)                         │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│   协调者                       参与者A        参与者B        │
│     │                             │             │           │
│     │────── Phase 1: Prepare ────►│             │           │
│     │────────────────────────────►│────────────►│           │
│     │                             │             │           │
│     │◄──────── Yes/No ────────────│             │           │
│     │◄────────────────────────────│─────────────│           │
│     │                             │             │           │
│     │────── Phase 2: Commit ─────►│             │           │
│     │────────────────────────────►│────────────►│           │
│     │   (如果所有Yes)             │             │           │
│     │                             │             │           │
│     │◄──────── ACK ───────────────│             │           │
│     │◄────────────────────────────│─────────────│           │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

```c
/* two_phase_commit.h */
#ifndef TWO_PHASE_COMMIT_H
#define TWO_PHASE_COMMIT_H

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    TPC_PHASE_IDLE = 0,
    TPC_PHASE_PREPARE,
    TPC_PHASE_COMMIT,
    TPC_PHASE_ABORT,
    TPC_PHASE_COMPLETED
} tpc_phase_t;

typedef enum {
    TPC_VOTE_UNKNOWN = 0,
    TPC_VOTE_YES,
    TPC_VOTE_NO
} tpc_vote_t;

typedef struct {
    uint64_t transaction_id;
    tpc_phase_t phase;
    uint32_t participant_count;
    uint32_t yes_votes;
    uint32_t no_votes;
    uint32_t timeout_ms;
    void *context;
} tpc_coordinator_t;

typedef struct {
    uint64_t transaction_id;
    bool prepared;
    bool committed;
    void (*prepare_handler)(void *context);
    void (*commit_handler)(void *context);
    void (*abort_handler)(void *context);
} tpc_participant_t;

/* 协调者API */
tpc_coordinator_t *tpc_coordinator_create(uint64_t tx_id, uint32_t participants);
int tpc_begin_prepare(tpc_coordinator_t *coord);
int tpc_send_commit(tpc_coordinator_t *coord);
int tpc_send_abort(tpc_coordinator_t *coord);

/* 参与者API */
int tpc_participant_vote(tpc_participant_t *p, tpc_vote_t vote);
int tpc_participant_prepare(tpc_participant_t *p);
int tpc_participant_commit(tpc_participant_t *p);
int tpc_participant_abort(tpc_participant_t *p);

#endif /* TWO_PHASE_COMMIT_H */
```

---

## 5. 分布式系统设计模式

### 5.1 常见模式

| 模式 | 描述 | 应用场景 |
|------|------|----------|
| **Circuit Breaker** | 熔断器模式，防止级联故障 | 服务调用保护 |
| **Bulkhead** | 舱壁隔离，限制资源使用 | 资源隔离 |
| **Retry with Backoff** | 指数退避重试 | 网络请求 |
| **Idempotency** | 幂等性保证 | 重复请求处理 |
| **Saga** | 长事务拆分 | 微服务事务 |
| **CQRS** | 读写分离 | 高并发场景 |
| **Event Sourcing** | 事件溯源 | 审计追踪 |

### 5.2 熔断器C语言实现

```c
/* circuit_breaker.h */
#ifndef CIRCUIT_BREAKER_H
#define CIRCUIT_BREAKER_H

#include <stdint.h>
#include <stdbool.h>
#include <time.h>

typedef enum {
    CB_STATE_CLOSED = 0,    /* 正常 - 关闭状态 */
    CB_STATE_OPEN,          /* 故障 - 打开状态 */
    CB_STATE_HALF_OPEN      /* 探测 - 半开状态 */
} circuit_breaker_state_t;

typedef struct {
    circuit_breaker_state_t state;

    /* 阈值配置 */
    uint32_t failure_threshold;     /* 故障阈值 */
    uint32_t success_threshold;     /* 成功阈值(半开状态) */
    uint32_t timeout_ms;            /* 超时时间 */

    /* 状态 */
    uint32_t failure_count;
    uint32_t success_count;
    uint64_t last_failure_time;

    /* 统计 */
    uint64_t total_requests;
    uint64_t total_failures;
    uint64_t total_successes;
} circuit_breaker_t;

circuit_breaker_t *cb_create(uint32_t failure_threshold,
                              uint32_t success_threshold,
                              uint32_t timeout_ms);
bool cb_allow_request(circuit_breaker_t *cb);
void cb_record_success(circuit_breaker_t *cb);
void cb_record_failure(circuit_breaker_t *cb);
const char *cb_state_string(circuit_breaker_state_t state);

#endif /* CIRCUIT_BREAKER_H */
```

```c
/* circuit_breaker.c */
#include "circuit_breaker.h"
#include <stdlib.h>

static uint64_t get_current_time_ms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}

circuit_breaker_t *cb_create(uint32_t failure_threshold,
                              uint32_t success_threshold,
                              uint32_t timeout_ms) {
    circuit_breaker_t *cb = calloc(1, sizeof(circuit_breaker_t));
    if (!cb) return NULL;

    cb->state = CB_STATE_CLOSED;
    cb->failure_threshold = failure_threshold;
    cb->success_threshold = success_threshold;
    cb->timeout_ms = timeout_ms;

    return cb;
}

bool cb_allow_request(circuit_breaker_t *cb) {
    if (!cb) return false;

    cb->total_requests++;

    switch (cb->state) {
        case CB_STATE_CLOSED:
            return true;

        case CB_STATE_OPEN:
            /* 检查是否超过超时时间 */
            if (get_current_time_ms() - cb->last_failure_time > cb->timeout_ms) {
                cb->state = CB_STATE_HALF_OPEN;
                cb->failure_count = 0;
                cb->success_count = 0;
                return true;
            }
            return false;

        case CB_STATE_HALF_OPEN:
            return true;
    }

    return false;
}

void cb_record_failure(circuit_breaker_t *cb) {
    if (!cb) return;

    cb->total_failures++;
    cb->failure_count++;
    cb->last_failure_time = get_current_time_ms();

    switch (cb->state) {
        case CB_STATE_CLOSED:
            if (cb->failure_count >= cb->failure_threshold) {
                cb->state = CB_STATE_OPEN;
            }
            break;

        case CB_STATE_HALF_OPEN:
            cb->state = CB_STATE_OPEN;
            break;

        case CB_STATE_OPEN:
            break;
    }
}

void cb_record_success(circuit_breaker_t *cb) {
    if (!cb) return;

    cb->total_successes++;

    switch (cb->state) {
        case CB_STATE_CLOSED:
            cb->failure_count = 0;
            break;

        case CB_STATE_HALF_OPEN:
            cb->success_count++;
            if (cb->success_count >= cb->success_threshold) {
                cb->state = CB_STATE_CLOSED;
                cb->failure_count = 0;
                cb->success_count = 0;
            }
            break;

        case CB_STATE_OPEN:
            break;
    }
}

const char *cb_state_string(circuit_breaker_state_t state) {
    switch (state) {
        case CB_STATE_CLOSED: return "CLOSED";
        case CB_STATE_OPEN: return "OPEN";
        case CB_STATE_HALF_OPEN: return "HALF_OPEN";
        default: return "UNKNOWN";
    }
}
```

---

## 6. 学习资源

| 类型 | 资源 | 说明 |
|------|------|------|
| 论文 | "In Search of an Understandable Consensus Algorithm" | Raft原始论文 |
| 论文 | "The Part-Time Parliament" | Paxos原始论文 |
| 书籍 | 《Designing Data-Intensive Applications》 | 分布式系统圣经 |
| 源码 | etcd / Consul | Raft实现参考 |
| 源码 | Redis Cluster | 分布式缓存实现 |

---

## 总结

分布式系统的核心挑战在于如何在网络分区、节点故障等异常情况下保持一致性和可用性。理解CAP定理、掌握一致性模型、实现共识算法是构建可靠分布式系统的基础。
